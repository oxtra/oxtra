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
		fprintf(assembly, "_start:\n\tlui t4, 0xf479e\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LUI(RiscVRegister::t4, 0xf479e);
		refactor(encoded);
	}

	SECTION("AUIPC") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tauipc t4, 0xf479e\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = AUIPC(RiscVRegister::t4, 0xf479e);
		refactor(encoded);
	}

	SECTION("JAL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tjal t4, 0xf479e\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = JAL(RiscVRegister::t4, 0xf479e);
		refactor(encoded);
	}

	SECTION("JALR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tjalr zero, ra, 0x0\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = JALR(RiscVRegister::zero, RiscVRegister::ra, 0x0);
		refactor(encoded);
	}

	SECTION("BEQ") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tbeq t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BEQ(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BNE") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tbne t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BNE(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BLT") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tblt t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BLT(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BGE") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tbge t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BGE(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BLTU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tbltu t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BLTU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("BGEU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tbgeu t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = BGEU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tlb t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LB(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tlh t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LH(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LW") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tlw t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LW(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tld t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LD(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LBU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tlbu t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LBU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LHU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tlhu t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LHU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("LWU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tlwu t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = LWU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsb t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SB(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsh t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SH(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SW") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsw t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SW(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsd t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SD(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("ADDI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\taddi t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = ADDI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SLTI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tslti t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLTI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SLTIU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsltiu t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLTIU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("XORI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\txori t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = XORI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("ORI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tori t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = ORI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("ANDI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tandi t4, t5, 0xf47\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = ANDI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		refactor(encoded);
	}

	SECTION("SLLI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tSLLI t4, t5, 0x11\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLLI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor(encoded);
	}

	SECTION("SRLI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tSRLI t4, t5, 0x11\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SRLI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor(encoded);
	}

	SECTION("SRAI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tSRAI t4, t5, 0x11\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SRAI(RiscVRegister::t4, RiscVRegister::t5, 0x11);
		refactor(encoded);
	}

	SECTION("ADD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tadd t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = ADD(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SUB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsub t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SUB(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SLL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsll t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SLT") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tslt t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLT(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SLTU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsltu t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SLTU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("XOR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\txor t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = XOR(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SRL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsrl t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SRL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("SRA") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tsra t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = SRA(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("OR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tor t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = OR(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("AND") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tand t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = AND(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("MUL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tmul t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = MUL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("MULH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tmulh t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = MULH(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("MULHSU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tmulhsu t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = MULHSU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("MULHU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tmulhu t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = MULHU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("DIV") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tdiv t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = DIV(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("DIVU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tdivu t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = DIVU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("REM") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\trem t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = REM(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	SECTION("REMU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "_start:\n\tremu t4, t5, t6\n");
		fclose(assembly);
		utils::riscv_instruction_t encoded = REMU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		refactor(encoded);
	}

	system("rm comp.*");
	system("rm dump");
}
