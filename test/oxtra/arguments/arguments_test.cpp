#include "../../catch2/catch.hpp"

#include "oxtra/arguments/arguments.h"

#include <string.h>

using namespace arguments;

TEST_CASE("Arguments support parsing valid arguments", "[arguments]") {
	SECTION("use default arguments") {
		constexpr const char* args_string[] = {"./oxtra", "x86app"};
		const auto arguments = Arguments(2, const_cast<char**>(args_string));

		REQUIRE(arguments.get_instruction_list_size() > 0);
		REQUIRE(arguments.get_entry_list_size() > 0);
		REQUIRE(arguments.get_offset_list_size() > 0);
		REQUIRE(strcmp(arguments.get_guest_path(), "x86app") == 0);
		REQUIRE(((int)arguments.get_log_level()) >= 0);
	}
	SECTION("use modified arguments") {
		constexpr const char* args_string[] = {"./oxtra", "-l", "5", "--linstruction-size=1337", "--lentry-size=42",
											   "--loffset-size=50", "app"};
		const auto arguments = Arguments(7, const_cast<char**>(args_string));

		REQUIRE(arguments.get_instruction_list_size() == 1337);
		REQUIRE(arguments.get_entry_list_size() == 42);
		REQUIRE(arguments.get_offset_list_size() == 50);
		REQUIRE(strcmp(arguments.get_guest_path(), "app") == 0);
		REQUIRE(((int)arguments.get_log_level()) == SPDLOG_LEVEL_CRITICAL);
	}
}