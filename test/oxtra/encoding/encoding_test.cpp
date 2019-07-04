#include "../../catch2/catch.hpp"
#include "oxtra/codegen/encoding/encoding.h"
#include <stdlib.h>
#include <string.h>

using namespace encoding;

void refactor(const char *assembly, utils::riscv_instruction_t encoded) {
	char assembly_[100] = {0};
	strcat(assembly_, ".section .text\n.global _start\n\n_start:\n\t");
	strcat(assembly_, assembly);
	strcat(assembly_, "\n");

	FILE *assemblyf = fopen("comp.s", "w");
	fprintf(assemblyf, "%s", assembly_);
	fclose(assemblyf);

	system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp.elf -nostdlib");
	system("objcopy -I elf64-little -j .text -O binary comp.elf dump");

	char* enc = reinterpret_cast<char*>(&encoded);

	char com[5] = {0};
	FILE* objdump = fopen("dump", "r");
	fread(com, 1, 4, objdump);
	fclose(objdump);

	REQUIRE(com[0] == enc[0]);
	REQUIRE(com[1] == enc[1]);
	REQUIRE(com[2] == enc[2]);
	REQUIRE(com[3] == enc[3]);
}

TEST_CASE("instruction encoding is correct", "[encoding]") {
	SECTION("LUI") {
		utils::riscv_instruction_t encoded = LUI(RiscVRegister::t4, 0xf48);
		refactor("lui x29, 0xf48", encoded);
	}

	SECTION("AUIPC") {
		utils::riscv_instruction_t encoded = AUIPC(RiscVRegister::t4, 0xf48);
		refactor("auipc x29, 0xf48", encoded);
	}

	SECTION("JAL") {
		utils::riscv_instruction_t encoded = JAL(RiscVRegister::t4, 0xf48);
		refactor("jal x29, 0xf48", encoded);
	}

	SECTION("JALR") {
		utils::riscv_instruction_t encoded = JALR(RiscVRegister::zero, RiscVRegister::ra, 0x0);
		refactor("jalr zero, ra, 0x0", encoded);
	}

	SECTION("BEQ") {
		utils::riscv_instruction_t encoded = BEQ(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("beq x29, x30, 0xf48", encoded);
	}

	SECTION("BNE") {
		utils::riscv_instruction_t encoded = BNE(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("bne x29, x30, 0xf48", encoded);
	}

	SECTION("BLT") {
		utils::riscv_instruction_t encoded = BLT(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("blt x29, x30, 0xf48", encoded);
	}

	SECTION("BGE") {
		utils::riscv_instruction_t encoded = BGE(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("bge x29, x30, 0xf48", encoded);
	}

	SECTION("BLTU") {
		utils::riscv_instruction_t encoded = BLTU(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("bltu x29, x30, 0xf48", encoded);
	}

	SECTION("BGEU") {
		utils::riscv_instruction_t encoded = BGEU(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("bgeu x29, x30, 0xf48", encoded);
	}

	SECTION("LB") {
		utils::riscv_instruction_t encoded = LB(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("lb x29, 0xf48(t5)", encoded);
	}

	SECTION("LH") {
		utils::riscv_instruction_t encoded = LH(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("lh x29, 0xf48(t5)", encoded);
	}

	SECTION("LW") {
		utils::riscv_instruction_t encoded = LW(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("lw x29, 0xf48(t5)", encoded);
	}

	SECTION("LD") {
		utils::riscv_instruction_t encoded = LD(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("ld x29, 0xf48(t5)", encoded);
	}

	SECTION("LBU") {
		utils::riscv_instruction_t encoded = LBU(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("lbu x29, 0xf48(t5)", encoded);
	}

	SECTION("LHU") {
		utils::riscv_instruction_t encoded = LHU(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("lhu x29, 0xf48(t5)", encoded);
	}

	SECTION("LWU") {
		utils::riscv_instruction_t encoded = LWU(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("lwu x29, 0xf48(t5)", encoded);
	}

	SECTION("SB") {
		utils::riscv_instruction_t encoded = SB(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("sb x29, 0xf48(t5)", encoded);
	}

	SECTION("SH") {
		utils::riscv_instruction_t encoded = SH(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("sh x29, 0xf48(t5)", encoded);
	}

	SECTION("SW") {
		utils::riscv_instruction_t encoded = SW(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("sw x29, 0xf48(t5)", encoded);
	}

	SECTION("SD") {
		utils::riscv_instruction_t encoded = SD(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("sd x29, 0xf48(t5)", encoded);
	}

	SECTION("ADDI") {
		utils::riscv_instruction_t encoded = ADDI(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("addi x29, x30, 0xf48", encoded);
	}

	SECTION("SLTI") {
		utils::riscv_instruction_t encoded = SLTI(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("slti x29, x30, 0xf48", encoded);
	}

	SECTION("SLTIU") {
		utils::riscv_instruction_t encoded = SLTIU(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("sltiu x29, x30, 0xf48", encoded);
	}

	SECTION("XORI") {
		utils::riscv_instruction_t encoded = XORI(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("xori x29, x30, 0xf48", encoded);
	}

	SECTION("ORI") {
		utils::riscv_instruction_t encoded = ORI(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("ori x29, x30, 0xf48", encoded);
	}

	SECTION("ANDI") {
		utils::riscv_instruction_t encoded = ANDI(RiscVRegister::t4, RiscVRegister::t5, 0xf48);
		refactor("andi x29, x30, 0xf48", encoded);
	}

	SECTION("SLLI") {
		utils::riscv_instruction_t encoded = SLLI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor("SLLI x29, x30, 0x11", encoded);
	}

	SECTION("SRLI") {
		utils::riscv_instruction_t encoded = SRLI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor("SRLI x29, x30, 0x11", encoded);
	}

	SECTION("SRAI") {
		utils::riscv_instruction_t encoded = SRAI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor("SRAI x29, x30, 0x11", encoded);
	}

	SECTION("ADD") {
		utils::riscv_instruction_t encoded = ADD(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("add x29, x30, x31", encoded);
	}

	SECTION("SUB") {
		utils::riscv_instruction_t encoded = SUB(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("sub x29, x30, x31", encoded);
	}

	SECTION("SLL") {
		utils::riscv_instruction_t encoded = SLL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("sll x29, x30, x31", encoded);
	}

	SECTION("SLT") {
		utils::riscv_instruction_t encoded = SLT(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("slt x29, x30, x31", encoded);
	}

	SECTION("SLTU") {
		utils::riscv_instruction_t encoded = SLTU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("sltu x29, x30, x31", encoded);
	}

	SECTION("XOR") {
		utils::riscv_instruction_t encoded = XOR(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("xor x29, x30, x31", encoded);
	}

	SECTION("SRL") {
		utils::riscv_instruction_t encoded = SRL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("srl x29, x30, x31", encoded);
	}

	SECTION("SRA") {
		utils::riscv_instruction_t encoded = SRA(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("sra x29, x30, x31", encoded);
	}

	SECTION("OR") {
		utils::riscv_instruction_t encoded = OR(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("or x29, x30, x31", encoded);
	}

	SECTION("AND") {
		utils::riscv_instruction_t encoded = AND(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("and x29, x30, x31", encoded);
	}

	SECTION("MUL") {
		utils::riscv_instruction_t encoded = MUL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("mul x29, x30, x31", encoded);
	}

	SECTION("MULH") {
		utils::riscv_instruction_t encoded = MULH(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("mulh x29, x30, x31", encoded);
	}

	SECTION("MULHSU") {
		utils::riscv_instruction_t encoded = MULHSU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("mulhsu x29, x30, x31", encoded);
	}

	SECTION("MULHU") {
		utils::riscv_instruction_t encoded = MULHU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("mulhu x29, x30, x31", encoded);
	}

	SECTION("DIV") {
		utils::riscv_instruction_t encoded = DIV(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("div x29, x30, x31", encoded);
	}

	SECTION("DIVU") {
		utils::riscv_instruction_t encoded = DIVU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("divu x29, x30, x31", encoded);
	}

	SECTION("REM") {
		utils::riscv_instruction_t encoded = REM(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("rem x29, x30, x31", encoded);
	}

	SECTION("REMU") {
		utils::riscv_instruction_t encoded = REMU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor("remu x29, x30, x31", encoded);
	}

	system("rm comp.*");
	system("rm dump");
}
