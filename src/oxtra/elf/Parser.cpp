#include "Parser.h"
using namespace elf;

//implementation of the ParserException-class
ParserException::ParserException() {
	_err = Error::undefined;
}
ParserException::ParserException(Error e) {
	_err = e;
}
const char* ParserException::what() const noexcept {
	switch (_err) {
		case Error::resource_failure:
			return "[elf::Parser::resource_failure]: failed to allocate the necessary memory";
		case Error::file_failed:
			return "[elf::Parser::file_failed]: failed to open or read the file";
		case Error::file_content:
			return "[elf::Parser::file_content]: this program can not handle the alignment of the contents of the file";
		case Error::not_elf_file:
			return "[elf::Parser::not_elf_file]: the file is not an elf-file";
		case Error::unsupported_binary:
			return "[elf::Parser::unsupported_binary]: the file does not conform to the binary-standards of this program";
		case Error::format_issue:
			return "[elf::Parser::format_issue]: this program can not handle this elf-format";
		case Error::no_content:
			return "[elf::Parser::no_content]: the file does not contain any mappable data";
		case Error::page_conflict:
			return "[elf::Parser::page_conflict]: two or more segments try to share a page with different flags";
		case Error::undefined:
		default:
			return "[elf::Parser::undefined]: undefined";
	}
}

//implementation of the internally used objects
Parser::Buffer::Buffer() {
	ptr = nullptr;
	size = 0;
}
Parser::Buffer::Buffer(void* p, uint64_t s) {
	ptr = p;
	size = s;
}
Parser::Buffer::~Buffer() {
	if(ptr != nullptr)
		free(ptr);
}

//implementation of the private-functions of the parser-class
Parser::Buffer Parser::read_file(const char* path) {
	//open the file
	FILE* file = fopen(path, "r");
	if(file == nullptr)
		throw ParserException(ParserException::file_failed);

	//read the size of the file
	fseek(file, 0, SEEK_END);
	uint64_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	if(size == 0) {
		fclose(file);
        throw ParserException(ParserException::file_failed);
    }

	//allocate a buffer for the file
	void* ptr =  malloc(size);
	if(ptr == nullptr) {
		fclose(file);
		throw ParserException(ParserException::resource_failure);
	}

	//read the contents of the file
	if(fread(ptr, 1, size, file) != size) {
		fclose(file);
		free(ptr);
		throw ParserException(ParserException::file_failed);
	}
	fclose(file);
	return Buffer(ptr, size);
}
void* Parser::find_in_file(Buffer* file, uint64_t addr, uint64_t size) {
    if(addr + size > file->size)
        return nullptr;
    return (void*)((uint64_t)file->ptr + addr);
}
void Parser::validate_elf(Buffer* file, uint8_t data_form, uint16_t machine, uint16_t type) {
	//get the pointer to the fileheader
	auto ehdr = reinterpret_cast<Elf64_Ehdr*>(find_in_file(file, 0, sizeof(Elf64_Ehdr)));
	if(ehdr == nullptr)
        throw ParserException(ParserException::file_content);

	//validate the header-identification
	if(memcmp(ehdr->e_ident, "\x7f" "ELF", 4) != 0)
        throw ParserException(ParserException::not_elf_file);
	if(ehdr->e_ident[EI_CLASS] != ELFCLASS64)
        throw ParserException(ParserException::unsupported_binary);
    if(ehdr->e_ident[EI_DATA] != data_form)
        throw ParserException(ParserException::unsupported_binary);
    if(ehdr->e_ident[EI_VERSION] != EV_CURRENT)
        throw ParserException(ParserException::format_issue);
	if(ehdr->e_ident[EI_OSABI] != ELFOSABI_LINUX)
        throw ParserException(ParserException::format_issue);
    if(ehdr->e_ident[EI_ABIVERSION] != 0)
        throw ParserException(ParserException::format_issue);

    //validate the type and machine of this binary
	if(ehdr->e_type != type)
        throw ParserException(ParserException::unsupported_binary);
    if(ehdr->e_machine != machine)
        throw ParserException(ParserException::unsupported_binary);

    //validate the rest of the parameter of the header
	if(ehdr->e_version != EV_CURRENT)
        throw ParserException(ParserException::format_issue);
    if(ehdr->e_entry == 0 && ehdr->e_type == ET_EXEC)
		throw ParserException(ParserException::unsupported_binary);
	if(ehdr->e_phoff == 0 || ehdr->e_phnum == 0 || ehdr->e_phentsize != sizeof(Elf64_Phdr))
        throw ParserException(ParserException::format_issue);
    if(ehdr->e_ehsize != sizeof(Elf64_Ehdr))
        throw ParserException(ParserException::format_issue);
}
void Parser::unpack_file(Buffer* file) {
    //extract the file-header and the entry-point
    auto ehdr = (Elf64_Ehdr*)find_in_file(file, 0, sizeof(Elf64_Ehdr));
    if(ehdr == nullptr)
    	throw ParserException(ParserException::file_content);
    _entry_point = ehdr->e_entry;

    //extract the number of program-headers/section-header & the index to the string-table
    uint64_t p_c = ehdr->e_phnum;
    uint64_t s_c = ehdr->e_shnum;
    uint64_t s_i = ehdr->e_shstrndx;
    if(ehdr->e_phnum == PN_XNUM || ehdr->e_shnum == 0 || s_i == SHN_XINDEX) {
        //extract the initial section-header
        auto i_shdr = (Elf64_Shdr*)find_in_file(file, ehdr->e_shoff, sizeof(Elf64_Shdr));
        if(i_shdr == 0)
			throw ParserException(ParserException::file_content);
        if(ehdr->e_phnum == PN_XNUM)
            p_c = i_shdr->sh_info;
        if(ehdr->e_shnum == 0)
            s_c = i_shdr->sh_size;
        if(s_i == SHN_XINDEX)
            s_i = i_shdr->sh_link;
    }
    if(p_c == 0 && s_c == 0)
		throw ParserException(ParserException::no_content);
	if(s_i >= s_c)
		throw ParserException(ParserException::file_content);

    //extract the program-header & section-header
    auto phdr = (Elf64_Phdr*)find_in_file(file, ehdr->e_phoff, p_c * ehdr->e_phentsize);
    auto shdr = (Elf64_Shdr*)find_in_file(file, ehdr->e_shoff, s_c * ehdr->e_shentsize);
    if((phdr == nullptr && p_c > 0) || (shdr == nullptr && s_c > 0))
		throw ParserException(ParserException::file_content);

    //get a pointer to the seciton-names and find the bss-section
    Elf64_Shdr* bss_section = nullptr;
    if(s_c > 0) {
        if(shdr[s_i].sh_type != SHT_STRTAB)
			throw ParserException(ParserException::file_content);

        //get the section-name-pointer
        auto name_ptr = (uint8_t*)find_in_file(file, shdr[s_i].sh_offset, shdr[s_i].sh_size);
        if(name_ptr == 0)
			throw ParserException(ParserException::file_content);

		//find the bss section
        for (uint64_t i = 0; i < s_c; i++) {
            if (shdr[i].sh_name >= shdr[s_i].sh_size)
				throw ParserException(ParserException::file_content);
			if(shdr[i].sh_name + 5 > shdr[s_i].sh_size)
                continue;
            if(memcmp(&name_ptr[shdr[i].sh_name], ".bss", 5) == 0) {
                bss_section = &shdr[i];
                if(bss_section->sh_type != SHT_NOBITS || ((bss_section->sh_flags & SHF_WRITE) == 0) || ((bss_section->sh_flags & SHF_ALLOC) == 0))
                    throw ParserException(ParserException::format_issue);
                break;
            }
        }
    }

    //extract the base-address and the total address-range
    _base_address = ~0x00u;
    _address_range = 0;
    for(uint64_t i = 0; i < p_c; i++) {
        if(phdr[i].p_type == PT_LOAD) {
            if(phdr[i].p_memsz < phdr[i].p_filesz)
				throw ParserException(ParserException::file_content);
			if(phdr[i].p_vaddr < _base_address)
                _base_address = phdr[i].p_vaddr;
            if(phdr[i].p_vaddr + phdr[i].p_memsz > _address_range)
                _address_range = phdr[i].p_vaddr + phdr[i].p_memsz;
        }
    }
    if(bss_section != 0) {
        if(bss_section->sh_addr < _base_address)
            _base_address = bss_section->sh_addr;
        if(bss_section->sh_addr + bss_section->sh_size > _address_range)
            _address_range = bss_section->sh_addr + bss_section->sh_size;
    }

    //page-align the base and the range
    _base_address &= ~0x00000FFFu;
    _address_range = ((_address_range & 0x00000FFFu) > 0 ? _address_range + 0x00001000 : _address_range) & ~0x00000FFFu;
    _address_range -= _base_address;
    if(_address_range == 0)
		throw ParserException(ParserException::file_content);

	//allocate the buffer for the image & the table for the page-entries
    _image_ptr = malloc(_address_range);
    if(_image_ptr == 0)
		throw ParserException(ParserException::resource_failure);
	_page_flags = (uint8_t*)malloc(_address_range >> 12u);
    if(_page_flags == 0) {
        free(_image_ptr);
		throw ParserException(ParserException::resource_failure);
    }
    memset(_page_flags, 0, _address_range >> 12u);
    memset(_image_ptr, 0, _address_range);

    //iterate through the program-headers and write the to memory
    for(uint64_t i = 0; i < p_c + 1; i++) {
        if (i < p_c ? phdr[i].p_type == PT_LOAD : true) {
            void* ptr = 0;
            if (i < p_c)
                ptr = find_in_file(file, phdr[i].p_offset, phdr[i].p_filesz);
            else if(bss_section == 0)
                break;
            else
                ptr = find_in_file(file, bss_section->sh_offset, bss_section->sh_size);
            if(ptr == 0) {
                free(_image_ptr);
                free(_page_flags);
				throw ParserException(ParserException::file_content);
			}

            //compute the first and the last page the blocks are within
            uint64_t page_start = ((i < p_c) ? phdr[i].p_vaddr : bss_section->sh_addr) - _base_address;
            uint64_t page_end = ((i < p_c) ? phdr[i].p_vaddr + phdr[i].p_memsz : bss_section->sh_addr + bss_section->sh_size) - _base_address;
            page_start >>= 12u;
            page_end = (page_end >> 12u) + ((page_end & 0x00000FFFu) > 0 ? 1 : 0);

            //extract the page-flags
            uint8_t flags = PAGE_MAPPED;
            if(i < p_c) {
                if (phdr[i].p_flags & PF_X)
                    flags |= PAGE_EXECUTE;
                if (phdr[i].p_flags & PF_W)
                    flags |= PAGE_WRITE;
                if (phdr[i].p_flags & PF_R)
                    flags |= PAGE_READ;
            }
            else
				flags |= PAGE_WRITE | PAGE_READ;

			//setup the page-flags
            for(uint64_t p = page_start; p < page_end; p++) {
                if(_page_flags[p] != 0 && _page_flags[p] != flags) {
                    free(_image_ptr);
                    free(_page_flags);
					throw ParserException(ParserException::page_conflict);
				}
                _page_flags[p] = flags;
            }

            //write the content to the memory
            if(i < p_c)
                memcpy((void*)((uint64_t)_image_ptr + phdr[i].p_vaddr - _base_address), ptr, phdr[i].p_filesz);
        }
    }
}

//implementation of the constructor of the parser-class
/**
 * Parses an elf-binary-file and unpacks the file
 * @param path path to the elf-binary-file
 * @param data_form little endian or big endian (default: ELFDATA2LSB)
 * @param machine architecture the parser looks for (default: EM_X86_64)
 * @param type the type of elf-binary (default: ET_EXEC)
 * @return returns false if already called before. Otherwise parses and returns true
 */
Parser::Parser(const char *path, uint8_t data_form, uint16_t machine, uint16_t type) {
	//initialize the this object
	_image_ptr 		= nullptr;
	_page_flags 	= nullptr;
	_base_address 	= 0;
	_address_range 	= 0;
	_entry_point 	= 0;

	//read the file
	Buffer file = read_file(path);

	//validate the file
	validate_elf(&file, data_form, machine, type);

	//unpack the file
	unpack_file(&file);
}
Parser::~Parser() {
    if(_image_ptr != nullptr)
    	free(_image_ptr);
    if(_page_flags != nullptr)
    	free(_page_flags);
}

//implementation of the public-functions of the parser-class
/**
 * retrieves the base-address of the image created by the binary-file
 * @return virtual base-address
 */
uint64_t Parser::get_base_vaddr() {
	return _base_address;
}
/**
 * retrieves the address to the first byte outside of the image created by the binary-file
 * @return virtual base-address + virtual address-range
 */
uint64_t Parser::get_highest_vaddr() {
	return _base_address + _address_range;
}
/**
 * retrieves the entry-point of the binary-image (might be zero, if not an executable)
 * @return virtual address of entry point
 */
uint64_t Parser::get_entry_point() {
	return _entry_point;
}
/**
 * resolves a virtual address to an actual pointer inside of the binary-image
 * (Pages within the virtual address-space of this binary, flagged as unmapped can be accessed but will be set to 0)
 * @param vaddr the virtual address
 * @return 0 if the virtual address lies outside of the addressable range of the binary, otherwise the pointer
 */
void* Parser::resolve_vaddr(uint64_t vaddr) {
	//validate the address
	vaddr -= _base_address;
	if(vaddr >= _address_range)
		return nullptr;
	return (void*)((uint64_t)_image_ptr + vaddr);
}
/**
 * extracts the page-flags of a given address inside of the binary-image
 * (PAGE_EXECUTE/PAGE_READ/PAGE_WRITE/PAGE_MAPPED)
 * @param vaddr the virtual address
 * @return 0 if the virtual address lies outside of the addressable range of the binary, otherwise the attributes
 */
uint8_t Parser::get_page_flags(uint64_t vaddr) {
	//validate the address
	vaddr -= _base_address;
	if(vaddr >= _address_range)
		return 0;
	return _page_flags[vaddr >> 12u];
}
/**
 * retrieves the maximum available memory for a given virtual address within the binary-image, which all share the
 * same page-flags, up to max_page's away from the virtual address
 * @param vaddr the virtual address
 * @param max_page the maximum number of pages to sample
 * @return number of bytes to end of max_page or change in page-flags
 */
uint64_t Parser::get_size(uint64_t vaddr, uint64_t max_page) {
	//validate the address
	vaddr -= _base_address;
	if(vaddr >= _address_range)
		return 0;

	//extract the page-flags
	uint8_t flags = _page_flags[vaddr >> 12u];

	//compute the initial size and convert vaddr to its page-index
	uint64_t size = vaddr & 0x00000FFFu;
	vaddr = (vaddr >> 12u) + (size > 0 ? 1 : 0);

	//compute the page-index of the last page to check and clip the value
	max_page += vaddr;
	if(max_page > (_address_range >> 12u))
		max_page = (_address_range >> 12u);

	//iterate through the pages and add their size up
	for(uint64_t i = vaddr; i < max_page; i++) {
		if(flags != _page_flags[i])
			return size;
		size += 0x1000;
	}
	return size;
}