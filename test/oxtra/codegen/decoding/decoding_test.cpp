#include "../../../catch2/catch.hpp"
#include "oxtra/codegen/encoding/encoding.h"
#include "oxtra/codegen/decoding/decoding.h"
#include <string>
#include <unistd.h>
#include <string.h>

using namespace encoding;
using namespace decoding;

constexpr const char* expected_rtype[] = {
		"sub rcx, rdx -> rbx",
		"add Vt2, rdx -> rbx",
		"mul rcx, rsp -> rbx",
		"rem rcx, Vgp -> rbx"
};
constexpr const char* expected_shift[] = {
		"slli rcx, 0x3f(63) -> rbx",
		"srli rcx, 0x1f(31) -> rbx",
		"slli rcx, 0x1(1) -> rbx",
		"srai rcx, 0x0(0) -> rbx"
};
constexpr const char* expected_itype[] = {
		"addi zero, 0x7ff(2047) -> rbx",
		"andi zero, 0x2(2) -> rbx",
		"ori zero, 0x0(0) -> rbx",
		"xori zero, -0x2(-2) -> rbx",
		"addi zero, -0x800(-2048) -> rbx"
};
constexpr const char* expected_utype[] = {
		"lui 0x7ffff(524287) -> rbx",
		"lui 0x2(2) -> rbx",
		"lui 0x0(0) -> rbx",
		"lui 0xffffe(1048574) -> rbx",
		"lui 0x80000(524288) -> rbx"
};
constexpr const char* expected_load[] = {
		"lb [rcx + 0x7ff(2047)] -> rbx",
		"lh [rcx + 0x2(2)] -> rbx",
		"lw [rcx] -> rbx",
		"ld [rcx + -0x2(-2)] -> rbx",
		"lhu [rcx + -0x800(-2048)] -> rbx"
};
constexpr const char* expected_store[] = {
		"sb rcx -> [rbx + 0x7ff(2047)]",
		"sh rcx -> [rbx + 0x2(2)]",
		"sw rcx -> [rbx]",
		"sd rcx -> [rbx + -0x2(-2)]",
		"sw rcx -> [rbx + -0x800(-2048)]"
};
constexpr const char* expected_jtype[] = {
		"jal $[pc + 0x7fffe(524286)] @ rbx",
		"jal $[pc + 0x2(2)] @ rbx",
		"jal $[pc] @ rbx",
		"jal $[pc + -0x2(-2)] @ rbx",
		"jal $[pc + -0x80000(-524288)] @ rbx"
};
constexpr const char* expected_relative[] = {
		"jalr $[rcx + 0x7ff(2047)] @ rbx",
		"jalr $[rcx + 0x2(2)] @ rbx",
		"jalr $[rcx] @ rbx",
		"jalr $[rcx + -0x2(-2)] @ rbx",
		"jalr $[rcx + -0x800(-2048)] @ rbx"
};
constexpr const char* expected_btype[] = {
		"beq rbx, rcx ? $[pc + 0x7fe(2046)]",
		"bne rbx, rcx ? $[pc + 0x2(2)]",
		"blt rbx, rcx ? $[pc]",
		"bge rbx, rcx ? $[pc + -0x2(-2)]",
		"bltu rbx, rcx ? $[pc + -0x800(-2048)]"
};

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
}