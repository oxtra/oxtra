#include "../../catch2/catch.hpp"

#include "oxtra/arguments/arguments.h"

#include <string.h>

using namespace arguments;

TEST_CASE("Arguments support parsing valid arguments", "[arguments]") {
	SECTION("use default arguments") {
		constexpr const char* args_string[] = {"./oxtra", "x86app"};
		const auto arguments = Arguments(2, const_cast<char**>(args_string));

		REQUIRE(arguments.get_guest_arguments().size() == 0);
		REQUIRE(arguments.get_instruction_list_size() > 0);
		REQUIRE(arguments.get_entry_list_size() > 0);
		REQUIRE(arguments.get_offset_list_size() > 0);
		REQUIRE(strcmp(arguments.get_guest_path(), "x86app") == 0);
		REQUIRE(((int)arguments.get_log_level()) >= 0);
		REQUIRE(((int)arguments.get_step_mode()) == (int)StepMode::none);
	}
	SECTION("use modified arguments") {
		constexpr const char* args_string[] = {"./oxtra", "-l", "5", "--stack-size=1338", "--linst-size=1337", "--lentry-size=42",
											   "--loffset-size=50", "app", "-a", "this is  a test", "--debug=RiscV"};
		const auto arguments = Arguments(11, const_cast<char**>(args_string));

		REQUIRE(arguments.get_guest_arguments().size() == 4);
		REQUIRE(arguments.get_guest_arguments()[0] == "this");
		REQUIRE(arguments.get_guest_arguments()[1] == "is");
		REQUIRE(arguments.get_guest_arguments()[2] == "a");
		REQUIRE(arguments.get_guest_arguments()[3] == "test");
		REQUIRE(arguments.get_stack_size() == 1338);
		REQUIRE(arguments.get_instruction_list_size() == 1337);
		REQUIRE(arguments.get_entry_list_size() == 42);
		REQUIRE(arguments.get_offset_list_size() == 50);
		REQUIRE(strcmp(arguments.get_guest_path(), "app") == 0);
		REQUIRE(((int)arguments.get_log_level()) == SPDLOG_LEVEL_CRITICAL);
		REQUIRE(((int)arguments.get_step_mode()) == (int)StepMode::riscv);
	}
	SECTION("nested quotes in string") {
		constexpr const char* args_string[] = {"./oxtra", "-a", "\"this is a \\\"  nested string\" -l  0", "app"};
		const auto arguments = Arguments(4, const_cast<char**>(args_string));

		REQUIRE(arguments.get_guest_arguments().size() == 3);
		REQUIRE(arguments.get_guest_arguments()[0] == "\"this is a \\\"  nested string\"");
		REQUIRE(arguments.get_guest_arguments()[1] == "-l");
		REQUIRE(arguments.get_guest_arguments()[2] == "0");
		REQUIRE(strcmp(arguments.get_guest_path(), "app") == 0);
	}
}