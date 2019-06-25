#include "elf.h"

using namespace elf;

const char* ElfException::error_string(Error e) {
	switch (e) {
		case Error::file_failed:
			return "[file_failed] failed to open or read the file";
		case Error::not_elf_file:
			return "[not_elf_file] the file is not an elf-file";
		case Error::unsupported_binary:
			return "[unsupported_binary] the file does not conform to the binary-standards of this program";
		case Error::unsupported_type:
			return "[unsupported_type] the file does not seem to be an executable";
		case Error::not_static:
			return "[not_static] the file has not been linked statically";
		case Error::format_issue:
			return "[format_issue] this program can not handle this elf-format";
		case Error::undefined:
		default:
			return "[undefined] undefined";
	}
}

ElfException::ElfException() {
	_err = Error::undefined;
	_info = std::string(error_string(_err));
}

ElfException::ElfException(Error e) {
	_err = e;
	_info = std::string(error_string(_err));
}

ElfException::ElfException(Error e, const char* info) {
	_err = e;
	_info = std::string(error_string(_err));
	_info.append("(desc: ").append(info).append(")");
}

const char* ElfException::what() const noexcept {
	return _info.c_str();
}

void Elf::read_file(const char* path) {
	//open the file
	FILE* file = fopen(path, "r");
	if (file == nullptr)
		throw ElfException(ElfException::file_failed, "open the file");

	//read the size of the file
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (size < sizeof(Elf64_Ehdr)) {
		fclose(file);
		throw ElfException(ElfException::file_failed, "file too small");
	}

	//allocate a buffer for the file
	_image_ptr = std::make_unique<uint8_t[]>(size);
	_address_range = size;

	//read the contents of the file
	if (fread(_image_ptr.get(), 1, size, file) != size) {
		fclose(file);
		throw ElfException(ElfException::file_failed, "read the file");
	}
	fclose(file);
}

template<typename tp>
tp Elf::resolve_offset(uintptr_t offset) {
	return reinterpret_cast<tp>(reinterpret_cast<uintptr_t>(_image_ptr.get()) + offset);
}

void Elf::validate_elf() {
	//get the pointer to the fileheader
	auto ehdr = resolve_offset<Elf64_Ehdr*>(0);

	//validate the header-identification
	if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0)
		throw ElfException(ElfException::not_elf_file, "invalid signature");
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
		throw ElfException(ElfException::unsupported_binary, "not 64-bit");
	if (ehdr->e_ident[EI_DATA] != ELFDATA2LSB)
		throw ElfException(ElfException::unsupported_binary, "not little endian");
	if (ehdr->e_ident[EI_VERSION] != EV_CURRENT)
		throw ElfException(ElfException::format_issue, "invalid version");
	if (ehdr->e_ident[EI_OSABI] != ELFOSABI_LINUX && ehdr->e_ident[EI_OSABI] != ELFOSABI_SYSV)
		throw ElfException(ElfException::format_issue, "invalid os-abi");
	if (ehdr->e_ident[EI_ABIVERSION] != 0)
		throw ElfException(ElfException::format_issue, "invalid abi-version");

	//validate the type and machine of this binary
	if (ehdr->e_type != ET_EXEC)
		throw ElfException(ElfException::unsupported_type, "not static executable");
	if (ehdr->e_machine != EM_X86_64)
		throw ElfException(ElfException::unsupported_binary, "invalid machine");

	//validate the rest of the parameter of the header
	if (ehdr->e_version != EV_CURRENT)
		throw ElfException(ElfException::format_issue, "invalid version");
	if (ehdr->e_entry == 0)
		throw ElfException(ElfException::unsupported_binary, "no entry-point");
	if (ehdr->e_phoff == 0 || ehdr->e_phnum == 0 || ehdr->e_phentsize != sizeof(Elf64_Phdr))
		throw ElfException(ElfException::format_issue, "program-header issue");
	if (ehdr->e_ehsize != sizeof(Elf64_Ehdr))
		throw ElfException(ElfException::format_issue, "exe-header unusable size");
}

void Elf::unpack_file() {
	//extract the file-header and the entry-point
	auto ehdr = resolve_offset<Elf64_Ehdr*>(0);
	_entry_point = ehdr->e_entry;

	//extract the number of program-headers/section-header & the index to the string-table
	size_t p_c = ehdr->e_phnum;
	size_t s_c = ehdr->e_shnum;
	size_t s_i = ehdr->e_shstrndx;
	if (ehdr->e_phnum == PN_XNUM || ehdr->e_shnum == 0 || s_i == SHN_XINDEX) {
		//extract the initial section-header
		auto shdr = resolve_offset<Elf64_Shdr*>(ehdr->e_shoff);
		if (ehdr->e_phnum == PN_XNUM)
			p_c = shdr->sh_info;
		if (ehdr->e_shnum == 0)
			s_c = shdr->sh_size;
		if (s_i == SHN_XINDEX)
			s_i = shdr->sh_link;
	}

	//extract the program-header & section-header
	auto phdr = resolve_offset<Elf64_Phdr*>(ehdr->e_phoff);
	auto shdr = resolve_offset<Elf64_Shdr*>(ehdr->e_shoff);

	//get a pointer to the seciton-names and find the bss-section
	Elf64_Shdr* bss_section = nullptr;
	if (s_c > 0) {
		//get the section-name-pointer and iterate through the sections
		auto name_ptr = resolve_offset<uint8_t*>(shdr[s_i].sh_offset);
		for (size_t i = 0; i < s_c; i++) {
			if (shdr[i].sh_name + 5 > shdr[s_i].sh_size)
				continue;
			if (memcmp(&name_ptr[shdr[i].sh_name], ".bss", 5) == 0) {
				bss_section = &shdr[i];
				break;
			}
		}
	}

	//check if the binary was statically linked
	for (size_t i = 0; i < p_c; i++) {
		if (phdr[i].p_type == PT_INTERP)
			throw ElfException(ElfException::not_static, "contains interpreter");
	}

	//extract the base-address and the total address-range
	_base_address = ~0x00u;
	size_t image_size = 0;
	for (size_t i = 0; i < p_c; i++) {
		if (phdr[i].p_type == PT_LOAD) {
			if (phdr[i].p_vaddr < _base_address)
				_base_address = phdr[i].p_vaddr;
			if (phdr[i].p_vaddr + phdr[i].p_memsz > image_size)
				image_size = phdr[i].p_vaddr + phdr[i].p_memsz;
		}
	}
	if (bss_section != nullptr) {
		if (bss_section->sh_addr < _base_address)
			_base_address = bss_section->sh_addr;
		if (bss_section->sh_addr + bss_section->sh_size > image_size)
			image_size = bss_section->sh_addr + bss_section->sh_size;
	}

	//page-align the base and the range
	_base_address ^= (_base_address & 0x00000FFFu);
	image_size = (image_size & 0x00000FFFu) > 0 ? image_size + 0x00001000 : image_size;
	image_size ^= (image_size & 0x00000FFFu);
	image_size -= _base_address;

	//allocate the buffer for the image & the table for the page-entries
	//(make the buffer larger by one page-size, allowing the image to be page-aligned)
	std::unique_ptr<uint8_t[]> image_buffer = std::make_unique<uint8_t[]>(image_size + 0x1000);
	_page_flags = std::make_unique<uint8_t[]>(image_size >> 12u);
	memset(image_buffer.get(), 0, image_size + 0x1000);
	memset(_page_flags.get(), 0, image_size >> 12u);

	//compute the actual base
	_actual_base = reinterpret_cast<uintptr_t>(image_buffer.get());
	if (_actual_base & 0x00000FFFu) {
		_actual_base += 0x1000;
		_actual_base ^= _actual_base & 0x00000FFFu;
	}

	//iterate through the program-headers and write them to memory
	for (size_t i = 0; i < p_c; i++) {
		if (phdr[i].p_type != PT_LOAD)
			continue;

		//compute the first and the last page the blocks are within
		size_t page_start = phdr[i].p_vaddr - _base_address;
		size_t page_end = phdr[i].p_vaddr + phdr[i].p_memsz - _base_address;
		page_start >>= 12u;
		page_end = (page_end >> 12u) + ((page_end & 0x00000FFFu) > 0 ? 1 : 0);

		//extract the page-flags
		uint8_t flags = PAGE_MAPPED;
		if (phdr[i].p_flags & PF_X) flags |= PAGE_EXECUTE;
		if (phdr[i].p_flags & PF_W) flags |= PAGE_WRITE;
		if (phdr[i].p_flags & PF_R) flags |= PAGE_READ;

		//setup the page-flags
		for (auto p = page_start; p < page_end; p++)
			_page_flags[p] = flags;

		//compute the source and destination-address as well as the copy-size
		uintptr_t copy_src = resolve_offset<uintptr_t>(phdr[i].p_offset);
		uintptr_t copy_dest = phdr[i].p_vaddr - _base_address + _actual_base;
		size_t copy_size = phdr[i].p_filesz;

		//align the source to be page-aligned
		if (phdr[i].p_vaddr & 0x00000FFFu) {
			uintptr_t offset = phdr[i].p_vaddr & 0x00000FFFu;
			copy_src -= offset;
			copy_dest -= offset;
			copy_size += offset;
		}

		//align the size to be page-aligned
		if (copy_size & 0x00000FFFu)
			copy_size += 0x1000;
		copy_size ^= (copy_size & 0x00000FFFu);

		//adjust the size (in order to not overrun the file-size)
		if (copy_src + copy_size > reinterpret_cast<uintptr_t>(_image_ptr.get()) + _address_range)
			copy_size = reinterpret_cast<uintptr_t>(_image_ptr.get()) + _address_range - copy_src;

		//map the section to memory
		memcpy(reinterpret_cast<void*>(copy_dest), reinterpret_cast<void*>(copy_src), copy_size);
	}

	//map the bss-section to pages
	if (bss_section != nullptr) {
		//compute the first and the last page the blocks are within
		size_t page_start = bss_section->sh_addr - _base_address;
		size_t page_end = bss_section->sh_addr + bss_section->sh_size - _base_address;
		page_start >>= 12u;
		page_end = (page_end >> 12u) + ((page_end & 0x00000FFFu) > 0 ? 1 : 0);

		//setup the page-flags
		uint8_t flags = PAGE_MAPPED | PAGE_READ | PAGE_WRITE;
		for (auto p = page_start; p < page_end; p++)
			_page_flags[p] = flags;
	}

	//switch the pointers and the size
	_image_ptr = std::move(image_buffer);
	_address_range = image_size;
}

Elf::Elf(const char* path) {
	//initialize the this object
	_image_ptr = nullptr;
	_page_flags = nullptr;
	_base_address = 0;
	_actual_base = 0;
	_address_range = 0;
	_entry_point = 0;

	//read the file
	read_file(path);

	//validate the file
	validate_elf();

	//unpack the file
	unpack_file();
}

uint8_t Elf::get_page_flags(uintptr_t vaddr) const {
	//validate the address
	vaddr -= _base_address;
	if (vaddr >= _address_range)
		return 0;
	return _page_flags[vaddr >> 12u];
}

size_t Elf::get_size(uintptr_t vaddr, size_t max_page) const {
	//validate the address
	vaddr -= _base_address;
	if (vaddr >= _address_range)
		return 0;

	//extract the page-flags
	uint8_t flags = _page_flags[vaddr >> 12u];

	//compute the initial size and convert vaddr to its page-index
	size_t size = vaddr & 0x00000FFFu;
	vaddr = (vaddr >> 12u) + (size > 0 ? 1 : 0);

	//compute the page-index of the last page to check and clip the value
	max_page += vaddr;
	if (max_page > (_address_range >> 12u))
		max_page = (_address_range >> 12u);

	//iterate through the pages and add their size up
	for (auto i = vaddr; i < max_page; i++) {
		if (flags != _page_flags[i])
			return size;
		size += 0x1000;
	}
	return size;
}
