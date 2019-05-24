#ifndef ELFPARSER_ELFPARSER_H
#define ELFPARSER_ELFPARSER_H
#include <inttypes.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <elf.h>
#include <exception>

//enter the namespace
namespace elf {
	//define the flags
	const uint8_t PAGE_EXECUTE 	= 0x01;
	const uint8_t PAGE_READ 	= 0x02;
	const uint8_t PAGE_WRITE 	= 0x04;
	const uint8_t PAGE_MAPPED 	= 0x08;

	//define the exception-class
	class ParserException : public std::exception {
		//define the publicly used object
	public:
		enum Error : uint8_t {
			undefined,
			resource_failure,
			file_failed,
			file_content,
			not_elf_file,
			format_issue,
			unsupported_binary,
			no_content,
			page_conflict
		};

		//define the private attributes
	private:
		Error _err;

		//define the constructors
	public:
		ParserException() ;
		ParserException(Error e);

		//define the public functions
	public:
		const char* what() const noexcept;
	};

	//define the parser-class
	class Parser {
		//define the internally used object
	private:
		struct Buffer {
			void* 		ptr;
			uint64_t 	size;
			Buffer();
			Buffer(void* p, uint64_t s);
			~Buffer();
		};

		//define the attributes
	private:
		void*           _image_ptr;
		uint64_t 		_base_address;
		uint64_t		_address_range;
		uint8_t*        _page_flags;
		uint64_t        _entry_point;

		//define the private functions
	private:
        static Buffer read_file(const char* path);
        static void* find_in_file(Buffer* file, uint64_t addr, uint64_t size);
        static void validate_elf(Buffer* file, uint8_t data_form, uint16_t machine, uint16_t type);
        void unpack_file(Buffer* file);

		//define the constructor and deconstructor
	public:
		Parser(const char *path, uint8_t data_form = ELFDATA2LSB, uint16_t machine = EM_X86_64, uint16_t type = ET_EXEC);
		~Parser();
		Parser() = delete;
		Parser(Parser&) = delete;
		Parser(Parser&&) = delete;

		//define the public functions
	public:
		uint64_t get_base_vaddr();
		uint64_t get_highest_vaddr();
		uint64_t get_entry_point();
		void* resolve_vaddr(uint64_t vaddr);
		uint8_t get_page_flags(uint64_t vaddr);
		uint64_t get_size(uint64_t vaddr, uint64_t max_page = 1);
	};
}
#endif //ELFPARSER_ELFPARSER_H
