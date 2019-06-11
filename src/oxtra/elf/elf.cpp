#include "elf.h"

using namespace elf;

Elf::Elf(const char* path) {

}

oxtra::guest_addr_t Elf::get_base_address() const {
	return 0;
}

size_t Elf::get_image_size() const {
	return 0;
}

oxtra::guest_addr_t Elf::get_entry_point() const {
	return 0;
}

uint8_t Elf::get_page_flags(oxtra::guest_addr_t vaddr) const {
	return 0;
}

void* Elf::get_addr(oxtra::guest_addr_t vaddr) const {
	return nullptr;
}