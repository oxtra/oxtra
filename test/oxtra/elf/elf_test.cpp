#define CATCH_CONFIG_MAIN

#include <oxtra/elf/elf.h>
#include "../../catch2/catch.hpp"
#include <string>
#include <unistd.h>

TEST_CASE("static Elf-Parser unpacking") {
	WARN("Expects resources to be in '.', '..', '../..' or './test', '../test', '../../test' - directory");

	//extract the base-path to use
	std::string base_path;
	if (access("./resources/dump_me", F_OK) != -1)
		base_path = "./resources";
	else if (access("./test/resources/dump_me", F_OK) != -1)
		base_path = "./test/resources";
	else if (access("../resources/dump_me", F_OK) != -1)
		base_path = "../resources";
	else if (access("../test/resources/dump_me", F_OK) != -1)
		base_path = "../test/resources";
	else if (access("../../resources/dump_me", F_OK) != -1)
		base_path = "../../resources";
	else if (access("../../test/resources/dump_me", F_OK) != -1)
		base_path = "../../test/resources";
	else
		FAIL("couldn't find resource-directory");

	//test the constructor
	REQUIRE_NOTHROW(elf::Elf((base_path
							+"/dump_me").c_str()));

	//read the file to compare against
	FILE* file = fopen((base_path + "/dump_me_raw_image").c_str(), "r");
	REQUIRE(file != nullptr);
	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);
	uint8_t* buffer = reinterpret_cast<uint8_t*>(malloc(size));
	REQUIRE(buffer != 0);

	//read the contents of the file
	REQUIRE(fread(buffer, 1, size, file) == size);
	fclose(file);

	//open the elf and extract the importent pointers
	elf::Elf elf((base_path + "/dump_me").c_str());
	uint8_t* target_ptr = buffer;
	uint8_t* actual_ptr = reinterpret_cast<uint8_t*>(elf.resolve_vaddr(elf.get_base_vaddr()));
	REQUIRE((elf.get_image_size() & 0x00000FFFu) == 0);

	//validate the parameter
	REQUIRE(elf.get_image_size() == size);
	REQUIRE(elf.get_entry_point() == 0x403fb0);
	REQUIRE(elf.get_base_vaddr() == 0x400000);
	REQUIRE((reinterpret_cast<uintptr_t>(elf.resolve_vaddr(elf.get_base_vaddr())) & 0x00000FFFu) == 0);

	//iterate through the non-writable pages and compare them
	for (size_t i = 0; i < elf.get_image_size(); i += 0x1000) {
		if ((elf.get_page_flags(i + elf.get_base_vaddr()) & elf::PAGE_WRITE) > 0 ||
			(elf.get_page_flags(i + elf.get_base_vaddr()) & elf::PAGE_MAPPED) == 0)
			continue;
		REQUIRE(memcmp(&target_ptr[i], &actual_ptr[i], 0x1000) == 0);
	}
}