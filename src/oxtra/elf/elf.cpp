#include "elf.h"

using namespace elf;
using namespace utils;

Elf::Elf(const char* path) {

}

guest_addr_t Elf::get_base_address() const {
	return 0;
}

size_t Elf::get_image_size() const {
	return 0;
}

guest_addr_t Elf::get_entry_point() const {
	return 0;
}

uint8_t Elf::get_page_flags(guest_addr_t vaddr) const {
	return 0;
}

void* Elf::get_addr(guest_addr_t vaddr) const {
	return nullptr;
}