#include "../../../catch2/catch.hpp"
#include "oxtra/codegen/encoding/encoding.h"
#include "oxtra/codegen/decoding/decoding.h"
#include <string>
#include <unistd.h>
#include <string.h>

using namespace encoding;
using namespace decoding;

constexpr const char* expected_rtype[] = {
		"sub rdx, r10 -> rsi",
		"add Vt2, r10 -> rsi",
		"mul rdx, rsp -> rsi",
		"rem rdx, Vgp -> rsi"
};
constexpr const char* expected_shift[] = {
		"slli rdx, 0x3f(63) -> rsi",
		"srli rdx, 0x1f(31) -> rsi",
		"slli rdx, 0x1(1) -> rsi",
		"srai rdx, 0x0(0) -> rsi"
};
constexpr const char* expected_itype[] = {
		"addi zero, 0x7ff(2047) -> rsi",
		"andi zero, 0x2(2) -> rsi",
		"ori zero, 0x0(0) -> rsi",
		"xori zero, -0x2(-2) -> rsi",
		"addi zero, -0x800(-2048) -> rsi"
};
constexpr const char* expected_utype[] = {
		"lui 0x7ffff(524287) -> rsi",
		"lui 0x2(2) -> rsi",
		"lui 0x0(0) -> rsi",
		"lui 0xffffe(1048574) -> rsi",
		"lui 0x80000(524288) -> rsi"
};
constexpr const char* expected_load[] = {
		"lb [rdx + 0x7ff(2047)] -> rsi",
		"lh [rdx + 0x2(2)] -> rsi",
		"lw [rdx] -> rsi",
		"ld [rdx - 0x2(2)] -> rsi",
		"lhu [rdx - 0x800(2048)] -> rsi"
};
constexpr const char* expected_store[] = {
		"sb rdx -> [rsi + 0x7ff(2047)]",
		"sh rdx -> [rsi + 0x2(2)]",
		"sw rdx -> [rsi]",
		"sd rdx -> [rsi - 0x2(2)]",
		"sw rdx -> [rsi - 0x800(2048)]"
};
constexpr const char* expected_jtype[] = {
		"jal [pc + 0x7fffe(524286)] -> rsi",
		"jal [pc + 0x2(2)] -> rsi",
		"jal [pc] -> rsi",
		"jal [pc - 0x2(2)] -> rsi",
		"jal [pc - 0x80000(524288)] -> rsi"
};
constexpr const char* expected_relative[] = {
		"jalr [rdx + 0x7ff(2047)] -> rsi",
		"jalr [rdx + 0x2(2)] -> rsi",
		"jalr [rdx] -> rsi",
		"jalr [rdx - 0x2(2)] -> rsi",
		"jalr [rdx - 0x800(2048)] -> rsi"
};
constexpr const char* expected_btype[] = {
		"beq (rsi, rdx) ? [pc + 0x7fe(2046)]",
		"bne (rsi, rdx) ? [pc + 0x2(2)]",
		"blt (rsi, rdx) ? [pc]",
		"bge (rsi, rdx) ? [pc - 0x2(2)]",
		"bltu (rsi, rdx) ? [pc - 0x800(2048)]"
};
constexpr const char* expected_ecall = "ecall";

TEST_CASE("decoding-test", "[decoding::parse_riscv]") {
	utils::riscv_instruction_t instruction[5];

	SECTION("r-type test") {
		instruction[0] = encoding::SUB(RiscVRegister::a1, RiscVRegister::a2, RiscVRegister::a3);
		instruction[1] = encoding::ADD(RiscVRegister::a1, RiscVRegister::t2, RiscVRegister::a3);
		instruction[2] = encoding::MUL(RiscVRegister::a1, RiscVRegister::a2, RiscVRegister::sp);
		instruction[3] = encoding::REM(RiscVRegister::a1, RiscVRegister::a2, RiscVRegister::gp);
		for (size_t i = 0; i < 4; i++)
			REQUIRE(parse_riscv(instruction[i]) == expected_rtype[i]);
	}
	SECTION("shift test") {
		instruction[0] = encoding::SLLI(RiscVRegister::a1, RiscVRegister::a2, 63);
		instruction[1] = encoding::SRLI(RiscVRegister::a1, RiscVRegister::a2, 31);
		instruction[2] = encoding::SLLI(RiscVRegister::a1, RiscVRegister::a2, 1);
		instruction[3] = encoding::SRAI(RiscVRegister::a1, RiscVRegister::a2, 0);
		for (size_t i = 0; i < 4; i++)
			REQUIRE(parse_riscv(instruction[i]) == expected_shift[i]);
	}
	SECTION("i-type test") {
		instruction[0] = encoding::ADDI(RiscVRegister::a1, RiscVRegister::zero, 0x7ff);
		instruction[1] = encoding::ANDI(RiscVRegister::a1, RiscVRegister::zero, 2);
		instruction[2] = encoding::ORI(RiscVRegister::a1, RiscVRegister::zero, 0);
		instruction[3] = encoding::XORI(RiscVRegister::a1, RiscVRegister::zero, -2);
		instruction[4] = encoding::ADDI(RiscVRegister::a1, RiscVRegister::zero, -0x800);
		for (size_t i = 0; i < 5; i++)
			REQUIRE(parse_riscv(instruction[i]) == expected_itype[i]);
	}
	SECTION("load-type test") {
		instruction[0] = encoding::LB(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 0x7ff);
		instruction[1] = encoding::LH(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 2);
		instruction[2] = encoding::LW(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 0);
		instruction[3] = encoding::LD(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, -2);
		instruction[4] = encoding::LHU(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, -0x800);
		for (size_t i = 0; i < 5; i++)
			REQUIRE(parse_riscv(instruction[i]) == expected_load[i]);
	}
	SECTION("store-type test") {
		instruction[0] = encoding::SB(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 0x7ff);
		instruction[1] = encoding::SH(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 2);
		instruction[2] = encoding::SW(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 0);
		instruction[3] = encoding::SD(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, -2);
		instruction[4] = encoding::SW(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, -0x800);
		for (size_t i = 0; i < 5; i++)
			REQUIRE(parse_riscv(instruction[i]) == expected_store[i]);
	}
	SECTION("j-type test") {
		instruction[0] = encoding::JAL(encoding::RiscVRegister::a1, 0x7ffff);
		instruction[1] = encoding::JAL(encoding::RiscVRegister::a1, 2);
		instruction[2] = encoding::JAL(encoding::RiscVRegister::a1, 0);
		instruction[3] = encoding::JAL(encoding::RiscVRegister::a1, -2);
		instruction[4] = encoding::JAL(encoding::RiscVRegister::a1, -0x80000);
		for (size_t i = 0; i < 5; i++)
			REQUIRE(parse_riscv(instruction[i]) == expected_jtype[i]);
	}
	SECTION("relative-type test") {
		instruction[0] = encoding::JALR(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 0x7ff);
		instruction[1] = encoding::JALR(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 2);
		instruction[2] = encoding::JALR(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 0);
		instruction[3] = encoding::JALR(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, -2);
		instruction[4] = encoding::JALR(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, -0x800);
		for (size_t i = 0; i < 5; i++)
			REQUIRE(parse_riscv(instruction[i]) == expected_relative[i]);
	}
	SECTION("b-type test") {
		instruction[0] = encoding::BEQ(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 0x7ff);
		instruction[1] = encoding::BNE(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 2);
		instruction[2] = encoding::BLT(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, 0);
		instruction[3] = encoding::BGE(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, -2);
		instruction[4] = encoding::BLTU(encoding::RiscVRegister::a1, encoding::RiscVRegister::a2, -0x800);
		for (size_t i = 0; i < 5; i++)
			REQUIRE(parse_riscv(instruction[i]) == expected_btype[i]);
	}
	SECTION("ecall test") {
		instruction[0] = encoding::ECALL();
		REQUIRE(parse_riscv(instruction[0]) == expected_ecall);
	}
}