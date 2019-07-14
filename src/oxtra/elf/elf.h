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
			not_static,
			base_address_taken
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

	class ElfImage {
	private:
		void* _base = nullptr;
		size_t _size = 0;

	public:
		ElfImage() = default;

		/**
		 * Creates an elf image.
		 * @param base_address The base address.
		 * @param size The size.
		 */
		explicit ElfImage(uintptr_t base_address, size_t size);

		~ElfImage();

		ElfImage(const ElfImage&) = delete;

		ElfImage(ElfImage&&) = default;

		ElfImage& operator=(const ElfImage&) = delete;

		ElfImage& operator=(ElfImage&&);

	public:
		/**
		 * Get the base address of the mapped image.
		 */
		void* get_base() const {
			return _base;
		}

		/**
		 * Get the size of the mapped image.
		 */
		size_t get_size() const {
			return _size;
		}
	};

	class Elf {
	private:
		ElfImage _image;
		std::unique_ptr<uint8_t[]> _page_flags;
		uintptr_t _entry_point;

	private:
		size_t read_file(const char* path);

		template<typename tp>
		tp resolve_offset(uintptr_t offset);

		void validate_elf();

		void unpack_file(size_t file_size);

	public:

		/**
 		* Parses an elf-binary-file and unpacks the file.
 		* @param path path to the elf-binary-file
 		* @return returns false if already called before. Otherwise parses and returns true
 		*/
		explicit Elf(const char* path);

		/**
		 * Creates a dummy-elf-object from a stream of bytes.
		 * By Default it will mark the first exe_pages-number of pages es read/executable.
		 * The rest of the pages will be flagged as read/writable.
		 * @param ptr stream of bytes
		 * @param size number of bytes in the stream
		 * @param base_address The base used to map the image to
		 * @param exe_pages number of initial pages flagged as executable
		 */
		explicit Elf(const uint8_t* ptr, size_t size, uintptr_t base_address = 0x00400000, size_t exec_pages = 1);

		Elf(const Elf&) = delete;

		Elf(Elf&&) = delete;

	public:
		/**
 		* retrieves the base-address of the image created by the binary-file
 		* @return virtual base-address
 		*/
		uintptr_t get_base_vaddr() const {
			return reinterpret_cast<uintptr_t>(_image.get_base());
		}

		/**
 		* retrieves the total size of the loaded and unpacked binary-file
 		* @return virtual address-range
	 	*/
		uintptr_t get_image_size() const {
			return _image.get_size();
		}

		/**
 		* retrieves the entry-point of the binary-image (might be zero, if not an executable)
 		* @return virtual address of entry point
 		*/
		uintptr_t get_entry_point() const {
			return _entry_point;
		}

		/**
 		* extracts the page-flags of a given address inside of the binary-image
 		* (PAGE_EXECUTE/PAGE_READ/PAGE_WRITE/PAGE_MAPPED)
 		* @param vaddr the virtual address
 		* @return 0 if the virtual address lies outside of the addressable range of the binary, otherwise the attributes
 		*/
		uint8_t get_page_flags(uintptr_t vaddr) const;

		/**
 		* retrieves the maximum available memory for a given virtual address within the binary-image, which all share the
 		* same page-flags, up to max_page's away from the virtual address
 		* @param vaddr the virtual address
 		* @param max_page the maximum number of pages to sample
 		* @return number of bytes to end of max_page or change in page-flags
 		*/
		size_t get_size(uintptr_t vaddr, size_t max_page = 1) const;
	};
}
#endif //OXTRA_ELF_H
