#include "../../catch2/catch.hpp"
#include "oxtra/codegen/encoding/encoding.h"
#include <stdlib.h>

using namespace encoding;

void refactor(utils::riscv_instruction_t encoded) {
		char* enc = reinterpret_cast<char*>(&encoded);
		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-as comp.s -o comp.o");
		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-ld comp.o -o comp.elf");
		system("objcopy -I elf64-little -j .text -O binary comp.elf dump");

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
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lui t4, 0xf479e");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LUI(RiscVRegister::t4, 0xf479e);
		refactor(encoded);
	}

	SECTION("AUIPC") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "auipc t4, 0xf479e");
		fclose(assembly);
		utils::riscv_instruction_t encoded = AUIPC(RiscVRegister::t4, 0xf479e);
		refactor(encoded);
	}

	SECTION("JAL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "jal t4, 0xf479e");
		fclose(assembly);
		utils::riscv_instruction_t encoded = JAL(RiscVRegister::t4, 0xf479e);
		refactor(encoded);
	}

	SECTION("JALR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "jalr zero, ra, 0x0");
		fclose(assembly);
		utils::riscv_instruction_t encoded = JALR(RiscVRegister::zero, RiscVRegister::ra, 0x0);
		refactor(encoded);
	}

	SECTION("BEQ") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "beq t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BEQ(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BNE") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "bne t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BNE(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BLT") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "blt t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BLT(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BGE") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "bge t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BGE(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BLTU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "bltu t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BLTU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BGEU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "bgeu t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BGEU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lb t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LB(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lh t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LH(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LW") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lw t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LW(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "ld t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LD(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LBU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lbu t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LBU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LHU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lhu t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LHU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LWU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lwu t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LWU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sb t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SB(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sh t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SH(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SW") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sw t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SW(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sd t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SD(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("ADDI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "addi t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = ADDI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SLTI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "slti t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLTI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SLTIU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sltiu t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLTIU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("XORI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "xori t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = XORI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("ORI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "ori t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = ORI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("ANDI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "andi t4, t5, 0xf47");
		fclose(assembly);
		utils::riscv_instruction_t encoded = ANDI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SLLI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "SLLI t4, t5, 0x11");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLLI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor(encoded);
	}

	SECTION("SRLI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "SRLI t4, t5, 0x11");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SRLI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor(encoded);
	}

	SECTION("SRAI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "SRAI t4, t5, 0x11");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SRAI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor(encoded);
	}

	SECTION("ADD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "add t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = ADD(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SUB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sub t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SUB(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SLL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sll t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SLT") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "slt t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLT(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SLTU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sltu t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLTU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("XOR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "xor t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = XOR(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SRL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "srl t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SRL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SRA") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sra t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SRA(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("OR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "or t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = OR(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("AND") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "and t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = AND(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("MUL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "mul t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = MUL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("MULH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "mulh t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = MULH(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("MULHSU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "mulhsu t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = MULHSU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("MULHU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "mulhu t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = MULHU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("DIV") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "div t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = DIV(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("DIVU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "divu t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = DIVU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("REM") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "rem t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = REM(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("REMU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "remu t4, t5, t6");
		fclose(assembly);
		utils::riscv_instruction_t encoded = REMU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	system("rm comp.*");
	system("rm dump");
}
