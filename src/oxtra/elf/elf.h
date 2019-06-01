#ifndef OXTRA_ELF_H
#define OXTRA_ELF_H

#include "oxtra/types.h"

namespace elf {
	class Elf {
	public:
		Elf(const char *path);

		Elf(Elf &) = delete;

		Elf(Elf &&) = delete;

	public:
		oxtra::virt_t get_base_address() const;

		size_t get_image_size() const;

		oxtra::virt_t get_entry_point() const;

		uint8_t get_page_flags(oxtra::virt_t vaddr) const;

		void *get_addr(oxtra::virt_t vaddr) const;
	};
}

#endif //OXTRA_ELF_H
