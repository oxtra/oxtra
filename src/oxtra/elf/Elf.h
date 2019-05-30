#ifndef OXTRA_ELF_H
#define OXTRA_ELF_H

#include <inttypes.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <elf.h>
#include <exception>
#include <memory>

//enter the namespace
namespace elf {
	//define the flags
	constexpr uint8_t PAGE_EXECUTE = 0x01;
	constexpr uint8_t PAGE_READ = 0x02;
	constexpr uint8_t PAGE_WRITE = 0x04;
	constexpr uint8_t PAGE_MAPPED = 0x08;

	//define the exception-class
	class ParserException : public std::exception {
	public:
		enum Error : uint8_t {
			undefined,
			file_failed,
			file_content,
			not_elf_file,
			format_issue,
			unsupported_binary,
			unsupported_type,
			not_static,
			no_content,
			page_conflict
		};

	private:
		Error _err;

	public:
		ParserException();

		ParserException(Error e);

	public:
		const char *what() const noexcept;
	};

	//define the parser-class
	class Elf {
	private:
		struct Buffer {
			std::unique_ptr<uint8_t[]> _ptr;
			size_t _size;

			Buffer(size_t size);
		};

	private:
		std::unique_ptr<uint8_t[]> _image_ptr;
		uintptr_t _base_address;
		size_t _address_range;
		std::unique_ptr<uint8_t[]> _page_flags;
		uintptr_t _entry_point;

	private:
		static Buffer read_file(const char *path);

		static void *find_in_file(Buffer *file, uintptr_t addr, size_t size);

		static void validate_elf(Buffer *file, uint8_t data_form, uint16_t machine);

		void unpack_file(Buffer *file);

	public:

		/**
 		* Parses an elf-binary-file and unpacks the file
 		* @param path path to the elf-binary-file
 		* @param data_form little endian or big endian (default: ELFDATA2LSB)
 		* @param machine architecture the parser looks for (default: EM_X86_64)
 		* @return returns false if already called before. Otherwise parses and returns true
 		*/
		Elf(const char *path, uint8_t data_form = ELFDATA2LSB, uint16_t machine = EM_X86_64);

		Elf() = delete;

		Elf(Elf &) = delete;

		Elf(Elf &&) = delete;

	public:
		/**
 		* retrieves the base-address of the image created by the binary-file
 		* @return virtual base-address
 		*/
		uintptr_t get_base_vaddr();

		/**
 		* retrieves the address to the first byte outside of the image created by the binary-file
 		* @return virtual base-address + virtual address-range
	 	*/
		uintptr_t get_highest_vaddr();

		/**
 		* retrieves the entry-point of the binary-image (might be zero, if not an executable)
 		* @return virtual address of entry point
 		*/
		uintptr_t get_entry_point();

		/**
 		* resolves a virtual address to an actual pointer inside of the binary-image
 		* (Pages within the virtual address-space of this binary, flagged as unmapped can be accessed but will be set to 0)
 		* @param vaddr the virtual address
 		* @return 0 if the virtual address lies outside of the addressable range of the binary, otherwise the pointer
 		*/
		void *resolve_vaddr(uintptr_t vaddr);

		/**
 		* extracts the page-flags of a given address inside of the binary-image
 		* (PAGE_EXECUTE/PAGE_READ/PAGE_WRITE/PAGE_MAPPED)
 		* @param vaddr the virtual address
 		* @return 0 if the virtual address lies outside of the addressable range of the binary, otherwise the attributes
 		*/
		uint8_t get_page_flags(uintptr_t vaddr);

		/**
 		* retrieves the maximum available memory for a given virtual address within the binary-image, which all share the
 		* same page-flags, up to max_page's away from the virtual address
 		* @param vaddr the virtual address
 		* @param max_page the maximum number of pages to sample
 		* @return number of bytes to end of max_page or change in page-flags
 		*/
		size_t get_size(uintptr_t vaddr, size_t max_page = 1);
	};
}
#endif //OXTRA_ELF_H