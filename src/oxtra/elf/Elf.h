#ifndef OXTRA_ELF_H
#define OXTRA_ELF_H

#include <inttypes.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <elf.h>
#include <exception>
#include <memory>

namespace elf {
	constexpr uint8_t PAGE_EXECUTE = 0x01u;
	constexpr uint8_t PAGE_READ = 0x02u;
	constexpr uint8_t PAGE_WRITE = 0x04u;
	constexpr uint8_t PAGE_MAPPED = 0x08u;

	class ElfException : public std::exception {
	public:
		enum Error : uint8_t {
			undefined,
			file_failed,
			not_elf_file,
			format_issue,
			unsupported_binary,
			unsupported_type,
			not_static
		};

	private:
		Error _err;
		std::string _info;

	private:
		static const char* error_string(Error e);

	public:
		ElfException();

		ElfException(Error e);

		ElfException(Error e, const char* info);

	public:
		const char* what() const noexcept;
	};

	class Elf {
	private:
		std::unique_ptr<uint8_t[]> _image_ptr;
		uintptr_t _actual_base;
		uintptr_t _base_address;
		size_t _address_range;
		std::unique_ptr<uint8_t[]> _page_flags;
		uintptr_t _entry_point;

	private:
		void read_file(const char* path);

		template<typename tp>
		tp resolve_offset(uintptr_t offset);

		void validate_elf();

		void unpack_file();

	public:

		/**
 		* Parses an elf-binary-file and unpacks the file
 		* @param path path to the elf-binary-file
 		* @return returns false if already called before. Otherwise parses and returns true
 		*/
		Elf(const char* path);

		Elf() = delete;

		Elf(Elf&) = delete;

		Elf(Elf&&) = delete;

	public:
		/**
 		* retrieves the base-address of the image created by the binary-file
 		* @return virtual base-address
 		*/
		uintptr_t get_base_vaddr() {
			return _base_address;
		}

		/**
 		* retrieves the total size of the loaded and unpacked binary-file
 		* @return virtual address-range
	 	*/
		uintptr_t get_image_size() {
			return _address_range;
		}

		/**
 		* retrieves the entry-point of the binary-image (might be zero, if not an executable)
 		* @return virtual address of entry point
 		*/
		uintptr_t get_entry_point() {
			return _entry_point;
		}

		/**
 		* retrieves the delta between the virtual address and the actual address
 		* @return _actual_base - _base_address
 		*/
		uintptr_t get_address_delta() {
			return _actual_base - _base_address;
		}

		/**
 		* resolves a virtual address to an actual pointer inside of the binary-image
 		* (Pages within the virtual address-space of this binary, flagged as unmapped can be accessed but will be set to 0)
 		* @param vaddr the virtual address
 		* @return 0 if the virtual address lies outside of the addressable range of the binary, otherwise the pointer
 		*/
		void* resolve_vaddr(uintptr_t vaddr);

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
