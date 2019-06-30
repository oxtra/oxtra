#include "../../../catch2/catch.hpp"
#include "oxtra/codegen/encoding/encoding.h"
#include "oxtra/codegen/decoding/decoding.h"
#include <string>
#include <unistd.h>
#include <string.h>

using namespace encoding;
using namespace decoding;

constexpr const char* expected_rtype[] = {
		"sub a2, a3 -> a1", "add t2, a3 -> a1", "mul a2, sp -> a1", "rem a2, zero -> a1"
};

TEST_CASE("decoding-test r-type", "[decoding::parse_riscv]") {
	utils::riscv_instruction_t instruction = encoding::SUB(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2,
														   encoding::RiscVRegister::a3);
	REQUIRE(parse_riscv(instruction).compare(expected_rtype[0]) == 0);
	instruction = encoding::ADD(encoding::RiscVRegister::a1, encoding::RiscVRegister::t2,
								encoding::RiscVRegister::a3);
	REQUIRE(parse_riscv(instruction).compare(expected_rtype[1]) == 0);
	instruction = encoding::MUL(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2,
								encoding::RiscVRegister::sp);
	REQUIRE(parse_riscv(instruction).compare(expected_rtype[2]) == 0);
	instruction = encoding::REM(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2,
								encoding::RiscVRegister::zero);
	REQUIRE(parse_riscv(instruction).compare(expected_rtype[3]) == 0);
}