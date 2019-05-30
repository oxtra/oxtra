#include "Elf.h"

using namespace elf;

//implementation of the ParserException-class
ParserException::ParserException() {
	_err = Error::undefined;
}

ParserException::ParserException(Error e) {
	_err = e;
}

const char *ParserException::what() const noexcept {
	switch (_err) {
		case Error::file_failed:
			return "[file_failed] failed to open or read the file";
		case Error::file_content:
			return "[file_content] this program can not handle the alignment of the contents of the file";
		case Error::not_elf_file:
			return "[not_elf_file] the file is not an elf-file";
		case Error::unsupported_binary:
			return "[unsupported_binary] the file does not conform to the binary-standards of this program";
		case Error::unsupported_type:
			return "[unsupported_type] the file does not seem to be an executable";
		case Error::format_issue:
			return "[format_issue] this program can not handle this elf-format";
		case Error::no_content:
			return "[no_content] the file does not contain any mappable data";
		case Error::page_conflict:
			return "[page_conflict] two or more segments try to share a page with different flags";
		case Error::undefined:
		default:
			return "[undefined] undefined";
	}
}

//implementation of the internally used objects
Elf::Buffer::Buffer(size_t size) {
	_ptr = std::make_unique<uint8_t[]>(size);
	_size = size;
}

//implementation of the private-functions of the parser-class
Elf::Buffer Elf::read_file(const char *path) {
	//open the file
	FILE *file = fopen(path, "r");
	if (file == nullptr)
		throw ParserException(ParserException::file_failed);

	//read the size of the file
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	if (size == 0) {
		fclose(file);
		throw ParserException(ParserException::file_failed);
	}

	//allocate a buffer for the file
	Buffer buf(size);

	//read the contents of the file
	if (fread(buf._ptr.get(), 1, size, file) != size) {
		fclose(file);
		throw ParserException(ParserException::file_failed);
	}
	fclose(file);
	return buf;
}

void *Elf::find_in_file(Buffer *file, uintptr_t addr, size_t size) {
	if (addr + size > file->_size)
		return nullptr;
	return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(file->_ptr.get()) + addr);
}

void Elf::validate_elf(Buffer *file, uint8_t data_form, uint16_t machine) {
	//get the pointer to the fileheader
	auto ehdr = reinterpret_cast<Elf64_Ehdr *>(find_in_file(file, 0, sizeof(Elf64_Ehdr)));
	if (ehdr == nullptr)
		throw ParserException(ParserException::file_content);

	//validate the header-identification
	if (memcmp(ehdr->e_ident, "\x7f" "Elf", 4) != 0)
		throw ParserException(ParserException::not_elf_file);
	if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
		throw ParserException(ParserException::unsupported_binary);
	if (ehdr->e_ident[EI_DATA] != data_form)
		throw ParserException(ParserException::unsupported_binary);
	if (ehdr->e_ident[EI_VERSION] != EV_CURRENT)
		throw ParserException(ParserException::format_issue);
	if (ehdr->e_ident[EI_OSABI] != ELFOSABI_LINUX && ehdr->e_ident[EI_OSABI] != ELFOSABI_SYSV)
		throw ParserException(ParserException::format_issue);
	if (ehdr->e_ident[EI_ABIVERSION] != 0)
		throw ParserException(ParserException::format_issue);

	//validate the type and machine of this binary
	if (ehdr->e_type != ET_EXEC)
		throw ParserException(ParserException::unsupported_type);
	if (ehdr->e_machine != machine)
		throw ParserException(ParserException::unsupported_binary);

	//validate the rest of the parameter of the header
	if (ehdr->e_version != EV_CURRENT)
		throw ParserException(ParserException::format_issue);
	if (ehdr->e_entry == 0)
		throw ParserException(ParserException::unsupported_binary);
	if (ehdr->e_phoff == 0 || ehdr->e_phnum == 0 || ehdr->e_phentsize != sizeof(Elf64_Phdr))
		throw ParserException(ParserException::format_issue);
	if (ehdr->e_ehsize != sizeof(Elf64_Ehdr))
		throw ParserException(ParserException::format_issue);
}

void Elf::unpack_file(Buffer *file) {
	//extract the file-header and the entry-point
	auto ehdr = reinterpret_cast<Elf64_Ehdr *>(find_in_file(file, 0, sizeof(Elf64_Ehdr)));
	if (ehdr == nullptr)
		throw ParserException(ParserException::file_content);
	_entry_point = ehdr->e_entry;

	//extract the number of program-headers/section-header & the index to the string-table
	size_t p_c = ehdr->e_phnum;
	size_t s_c = ehdr->e_shnum;
	size_t s_i = ehdr->e_shstrndx;
	if (ehdr->e_phnum == PN_XNUM || ehdr->e_shnum == 0 || s_i == SHN_XINDEX) {
		//extract the initial section-header
		auto shdr = reinterpret_cast<Elf64_Shdr *>(find_in_file(file, ehdr->e_shoff, sizeof(Elf64_Shdr)));
		if (shdr == nullptr)
			throw ParserException(ParserException::file_content);
		if (ehdr->e_phnum == PN_XNUM)
			p_c = shdr->sh_info;
		if (ehdr->e_shnum == 0)
			s_c = shdr->sh_size;
		if (s_i == SHN_XINDEX)
			s_i = shdr->sh_link;
	}
	if (p_c == 0 && s_c == 0)
		throw ParserException(ParserException::no_content);
	if (s_i >= s_c)
		throw ParserException(ParserException::file_content);

	//extract the program-header & section-header
	auto phdr = reinterpret_cast<Elf64_Phdr *>(find_in_file(file, ehdr->e_phoff, p_c * ehdr->e_phentsize));
	auto shdr = reinterpret_cast<Elf64_Shdr *>(find_in_file(file, ehdr->e_shoff, s_c * ehdr->e_shentsize));
	if ((phdr == nullptr && p_c > 0) || (shdr == nullptr && s_c > 0))
		throw ParserException(ParserException::file_content);

	//get a pointer to the seciton-names and find the bss-section
	Elf64_Shdr *bss_section = nullptr;
	if (s_c > 0) {
		if (shdr[s_i].sh_type != SHT_STRTAB)
			throw ParserException(ParserException::file_content);

		//get the section-name-pointer
		auto name_ptr = reinterpret_cast<uint8_t *>(find_in_file(file, shdr[s_i].sh_offset, shdr[s_i].sh_size));
		if (name_ptr == nullptr)
			throw ParserException(ParserException::file_content);

		//find the bss section
		for (auto i = 0; i < s_c; i++) {
			if (shdr[i].sh_name >= shdr[s_i].sh_size)
				throw ParserException(ParserException::file_content);
			if (shdr[i].sh_name + 5 > shdr[s_i].sh_size)
				continue;
			if (memcmp(&name_ptr[shdr[i].sh_name], ".bss", 5) == 0) {
				bss_section = &shdr[i];
				if (bss_section->sh_type != SHT_NOBITS || ((bss_section->sh_flags & SHF_WRITE) == 0) ||
					((bss_section->sh_flags & SHF_ALLOC) == 0))
					throw ParserException(ParserException::format_issue);
				break;
			}
		}
	}

	//extract the base-address and the total address-range
	_base_address = ~0x00u;
	_address_range = 0;
	for (auto i = 0; i < p_c; i++) {
		if (phdr[i].p_type == PT_LOAD) {
			if (phdr[i].p_memsz < phdr[i].p_filesz)
				throw ParserException(ParserException::file_content);
			if (phdr[i].p_vaddr < _base_address)
				_base_address = phdr[i].p_vaddr;
			if (phdr[i].p_vaddr + phdr[i].p_memsz > _address_range)
				_address_range = phdr[i].p_vaddr + phdr[i].p_memsz;
		}
	}
	if (bss_section != nullptr) {
		if (bss_section->sh_addr < _base_address)
			_base_address = bss_section->sh_addr;
		if (bss_section->sh_addr + bss_section->sh_size > _address_range)
			_address_range = bss_section->sh_addr + bss_section->sh_size;
	}

	//page-align the base and the range
	_base_address &= ~0x00000FFFu;
	_address_range = ((_address_range & 0x00000FFFu) > 0 ? _address_range + 0x00001000 : _address_range) & ~0x00000FFFu;
	_address_range -= _base_address;
	if (_address_range == 0)
		throw ParserException(ParserException::file_content);

	//allocate the buffer for the image & the table for the page-entries
	_image_ptr = std::make_unique<uint8_t[]>(_address_range);
	_page_flags = std::make_unique<uint8_t[]>(_address_range >> 12u);
	memset(_image_ptr.get(), 0, _address_range);
	memset(_page_flags.get(), 0, _address_range >> 12u);

	//iterate through the program-headers and write the to memory
	for (auto i = 0; i < p_c + 1; i++) {
		if (i < p_c ? phdr[i].p_type == PT_LOAD : true) {
			void *ptr = nullptr;
			if (i < p_c)
				ptr = find_in_file(file, phdr[i].p_offset, phdr[i].p_filesz);
			else if (bss_section == nullptr)
				break;
			else
				ptr = find_in_file(file, bss_section->sh_offset, bss_section->sh_size);
			if (ptr == nullptr)
				throw ParserException(ParserException::file_content);

			//compute the first and the last page the blocks are within
			size_t page_start = ((i < p_c) ? phdr[i].p_vaddr : bss_section->sh_addr) - _base_address;
			size_t page_end =
					((i < p_c) ? phdr[i].p_vaddr + phdr[i].p_memsz : bss_section->sh_addr + bss_section->sh_size) -
					_base_address;
			page_start >>= 12u;
			page_end = (page_end >> 12u) + ((page_end & 0x00000FFFu) > 0 ? 1 : 0);

			//extract the page-flags
			uint8_t flags = PAGE_MAPPED;
			if (i < p_c) {
				if (phdr[i].p_flags & PF_X)
					flags |= PAGE_EXECUTE;
				if (phdr[i].p_flags & PF_W)
					flags |= PAGE_WRITE;
				if (phdr[i].p_flags & PF_R)
					flags |= PAGE_READ;
			} else
				flags |= PAGE_WRITE | PAGE_READ;

			//setup the page-flags
			for (auto p = page_start; p < page_end; p++) {
				if (_page_flags[p] != 0 && _page_flags[p] != flags)
					throw ParserException(ParserException::page_conflict);
				_page_flags[p] = flags;
			}

			//write the content to the memory
			if (i < p_c)
				memcpy(reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(_image_ptr.get()) + phdr[i].p_vaddr -
												_base_address), ptr, phdr[i].p_filesz);
		}
	}
}

//implementation of the constructor of the parser-class
Elf::Elf(const char *path, uint8_t data_form, uint16_t machine) {
	//initialize the this object
	_image_ptr = nullptr;
	_page_flags = nullptr;
	_base_address = 0;
	_address_range = 0;
	_entry_point = 0;

	//read the file
	Buffer file = read_file(path);

	//validate the file
	validate_elf(&file, data_form, machine);

	//unpack the file
	unpack_file(&file);
}

//implementation of the public-functions of the parser-class
uintptr_t Elf::get_base_vaddr() {
	return _base_address;
}

uintptr_t Elf::get_highest_vaddr() {
	return _base_address + _address_range;
}

uintptr_t Elf::get_entry_point() {
	return _entry_point;
}

void *Elf::resolve_vaddr(uintptr_t vaddr) {
	//validate the address
	vaddr -= _base_address;
	if (vaddr >= _address_range)
		return nullptr;
	return reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(_image_ptr.get()) + vaddr);
}

uint8_t Elf::get_page_flags(uintptr_t vaddr) {
	//validate the address
	vaddr -= _base_address;
	if (vaddr >= _address_range)
		return 0;
	return _page_flags[vaddr >> 12u];
}

size_t Elf::get_size(uintptr_t vaddr, size_t max_page) {
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
	for (size_t i = vaddr; i < max_page; i++) {
		if (flags != _page_flags[i])
			return size;
		size += 0x1000;
	}
	return size;
}