#include "../../../catch2/catch.hpp"
#include "oxtra/codegen/codestore/codestore.h"
#include <string>
#include <unistd.h>
#include "oxtra/elf/elf.h"
#include "oxtra/arguments/arguments.h"

using namespace codegen;
using namespace codestore;

constexpr const char* search_strings[] = {"./resources", "./test/resources", "../resources", "../test/resources",
										 "../../resources", "../../test/resources"};
constexpr size_t search_strings_count = 6;
constexpr const char* test_path[] = { "/this/is/my/test/path" };
constexpr size_t test_path_count = 1;

TEST_CASE("codestore test instruction-adding", "[codestore]") {
	// extract the base-path to for the elf-object
	std::string base_path;
	for(size_t i = 0; i < search_strings_count; i++) {
		std::string temp_path(search_strings[i]);
		temp_path += "/dump_me";
		if(access(temp_path.c_str(), F_OK) != -1) {
			base_path = temp_path;
			break;
		}
	}
	if(base_path.empty())
		FAIL("couldn't find resource-directory");

	// create the elf-object and the arguments-object
	elf::Elf temp_elf = elf::Elf(base_path.c_str());
	arguments::Arguments temp_args = arguments::Arguments(test_path_count, const_cast<char**>(test_path));

	// create the code-store object
	CodeStore store = CodeStore(temp_args, temp_elf);

	SECTION("create an empty block") {
		BlockEntry& entry = store.create_block();

	}
	SECTION("add multiple instructions") {

	}
	SECTION("add multiple blocks and resolve blocks") {

	}

}
