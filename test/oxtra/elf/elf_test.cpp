#define CATCH_CONFIG_MAIN

#include <oxtra/elf/elf.h>
#include "../../catch2/catch.hpp"
#include <string>
#include <unistd.h>
#include <time.h>

TEST_CASE("static Elf-Parser unpacking", "[elf::Elf]") {
	SECTION("Default constructor test") {
		WARN("Expects resources to be in '.', '..', '../..' or './test', '../test', '../../test' - directory");

		// extract the base-path to use
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

		// test the constructor
		REQUIRE_NOTHROW(elf::Elf((base_path
								+"/dump_me").c_str()));

		// read the file to compare against
		FILE* file = fopen((base_path + "/dump_me_raw_image").c_str(), "r");
		REQUIRE(file != nullptr);
		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		fseek(file, 0, SEEK_SET);
		uint8_t* buffer = reinterpret_cast<uint8_t*>(malloc(size));
		REQUIRE(buffer != 0);

		// read the contents of the file
		REQUIRE(fread(buffer, 1, size, file) == size);
		fclose(file);

		// open the elf and extract the important pointers
		elf::Elf elf((base_path + "/dump_me").c_str());
		uint8_t* target_ptr = buffer;
		uint8_t* actual_ptr = reinterpret_cast<uint8_t*>(elf.get_base_vaddr());
		REQUIRE((elf.get_image_size() & 0x00000FFFu) == 0);

		// validate the parameter
		REQUIRE(elf.get_image_size() == size);
		REQUIRE(elf.get_entry_point() == 0x403fb0);
		REQUIRE(elf.get_base_vaddr() == 0x400000);

		// iterate through the non-writable pages and compare them
		for (size_t i = 0; i < elf.get_image_size(); i += 0x1000) {
			if ((elf.get_page_flags(i + elf.get_base_vaddr()) & elf::PAGE_WRITE) > 0 ||
				(elf.get_page_flags(i + elf.get_base_vaddr()) & elf::PAGE_MAPPED) == 0)
				continue;
			REQUIRE(memcmp(&target_ptr[i], &actual_ptr[i], 0x1000) == 0);
		}
	}
	SECTION("debug-constructor test"){
		// create a random 0x3400-bytes large block
		uint8_t random_buffer[0x3400];
		srand(time(0));
		for(size_t i = 0; i < sizeof(random_buffer); i++)
			random_buffer[i] = static_cast<uint8_t>(rand());

		// create the elf-object
		elf::Elf elf = elf::Elf(random_buffer, sizeof(random_buffer), 0x400000, 3);

		// validate the parameters
		REQUIRE(elf.get_base_vaddr() == 0x400000);
		REQUIRE(elf.get_entry_point() == 0x400000);
		REQUIRE(elf.get_image_size() == 0x4000);
		REQUIRE(elf.get_page_flags(0x400000) == (elf::PAGE_EXECUTE | elf::PAGE_MAPPED | elf::PAGE_READ));
		REQUIRE(elf.get_page_flags(0x401000) == elf.get_page_flags(0x400000));
		REQUIRE(elf.get_page_flags(0x402000) == elf.get_page_flags(0x400000));
		REQUIRE(elf.get_page_flags(0x403000) == (elf::PAGE_WRITE | elf::PAGE_MAPPED | elf::PAGE_READ));
		REQUIRE(elf.get_size(0x400050, 1) == 0x2000 - 0x50);
		REQUIRE(elf.get_size(0x400050, 4) == 0x3000 - 0x50);
		REQUIRE(elf.get_size(0x403100) == 0xf00);
		REQUIRE(memcmp(reinterpret_cast<void*>(elf.get_base_vaddr()), random_buffer, sizeof(random_buffer)) == 0);
	}
}
