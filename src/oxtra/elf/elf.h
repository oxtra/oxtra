#ifndef OXTRA_ELF_H
#define OXTRA_ELF_H

#include "oxtra/utils/types.h"

namespace elf {
	class Elf {
	public:
		Elf(const char* path);

		Elf(Elf&) = delete;

		Elf(Elf&&) = delete;

	public:
		utils::guest_addr_t get_base_address() const;

		size_t get_image_size() const;

		utils::guest_addr_t get_entry_point() const;

		uint8_t get_page_flags(utils::guest_addr_t vaddr) const;

		void* get_addr(utils::guest_addr_t vaddr) const;
	};
}

#endif //OXTRA_ELF_H
