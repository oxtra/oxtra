#include "../../catch2/catch.hpp"
#include "oxtra/codegen/encoding/encoding.h"
#include <stdlib.h>

using namespace encoding;

TEST_CASE("instruction encoding is correct", "[encoding]") {
	SECTION("LUI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lui t4, 0xf479e");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = LUI(RiscVRegister::t4, 0xf479e);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("AUIPC") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "auipc t4, 0xf479e");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = AUIPC(RiscVRegister::t4, 0xf479e);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("JAL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "jal t4, 0xf479e");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = JAL(RiscVRegister::t4, 0xf479e);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

#if 0
	SECTION("JALR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "jalr zero, ra, 0x0");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = JALR(RiscVRegister::zero, RiscVRegister::ra, 0x0);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}
#endif

	SECTION("BEQ") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "beq t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = BEQ(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("BNE") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "bne t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = BNE(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("BLT") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "blt t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = BLT(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("BGE") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "bge t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = BGE(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("BLTU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "bltu t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = BLTU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("BGEU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "bgeu t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = BGEU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("LB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lb t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = LB(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("LH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lh t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = LH(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("LW") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lw t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = LW(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("LD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "ld t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = LD(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("LBU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lbu t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = LBU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("LHU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lhu t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = LHU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("LWU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "lwu t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = LWU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sb t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SB(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sh t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SH(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SW") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sw t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SW(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sd t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SD(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("ADDI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "addi t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = ADDI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SLTI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "slti t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SLTI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SLTIU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sltiu t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SLTIU(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("XORI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "xori t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = XORI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("ORI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "ori t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = ORI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("ANDI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "andi t4, t5, 0xf47");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = ANDI(RiscVRegister::t4, RiscVRegister::t5, 0xf47);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SLLI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "SLLI t4, t5, 0xf1");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SLLI(RiscVRegister::t4, RiscVRegister::t5, 0xf1);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SRLI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "SRLI t4, t5, 0xf1");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SRLI(RiscVRegister::t4, RiscVRegister::t5, 0xf1);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SRAI") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "SRAI t4, t5, 0xf1");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SRAI(RiscVRegister::t4, RiscVRegister::t5, 0xf1);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("ADD") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "add t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = ADD(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SUB") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sub t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SUB(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SLL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sll t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SLL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SLT") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "slt t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SLT(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SLTU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sltu t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SLTU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("XOR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "xor t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = XOR(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SRL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "srl t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SRL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("SRA") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "sra t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = SRA(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("OR") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "or t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = OR(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("AND") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "and t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = AND(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("MUL") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "mul t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = MUL(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("MULH") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "mulh t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = MULH(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("MULHSU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "mulhsu t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = MULHSU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("MULHU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "mulhu t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = MULHU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("DIV") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "div t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = DIV(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("DIVU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "divu t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = DIVU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("REM") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "rem t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = REM(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}

	SECTION("REMU") {
		FILE* assembly = fopen("comp.s", "w");
		fprintf(assembly, "remu t4, t5, t6");
		fclose(assembly);

		utils::riscv_instruction_t encoded_ = REMU(RiscVRegister::t4, RiscVRegister::t5, RiscVRegister::t6);
		char* encoded = reinterpret_cast<char*>(&encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc comp.s -o comp -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary comp dump");

		char compiled[5] = {0};
		FILE* objdump = fopen("dump", "r");
		fread(compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(compiled[0] == encoded[0]);
		REQUIRE(compiled[1] == encoded[1]);
		REQUIRE(compiled[2] == encoded[2]);
		REQUIRE(compiled[3] == encoded[3]);
	}
}
