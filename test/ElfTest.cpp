#define CATCH_CONFIG_MAIN

#include <oxtra/elf/Elf.h>
#include "catch2/catch.hpp"

TEST_CASE("Elf-Parser self test") {
	WARN("- Expects this process to be static and loaded to 0x400000.\n"
		 "  - Dont run this program as debug, as gdb replaces some opcodes with '0xcc' [debug-break].\n"
		 "  - Expects this binary to be called 'unit_tests'");

	//test the constructor
	REQUIRE_NOTHROW(elf::Elf("./unit_tests"));

	//open the elf and extract the importent pointers
	elf::Elf elf("./unit_tests");
	uint8_t* target_ptr = reinterpret_cast<uint8_t*>(0x400000);
	uint8_t* actual_ptr = reinterpret_cast<uint8_t*>(elf.resolve_vaddr(elf.get_base_vaddr()));
	REQUIRE((elf.get_image_size() & 0x00000FFFu) == 0);

	//iterate through the non-writable pages and compare them
	for (auto i = 0; i < elf.get_image_size(); i += 0x1000) {
		if ((elf.get_page_flags(i + elf.get_base_vaddr()) & elf::PAGE_WRITE) > 0 ||
			(elf.get_page_flags(i + elf.get_base_vaddr()) & elf::PAGE_MAPPED) == 0)
			continue;
		REQUIRE(memcmp(&target_ptr[i], &actual_ptr[i], 0x1000) == 0);
	}
}
