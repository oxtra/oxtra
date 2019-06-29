#include "../../catch2/catch.hpp"
#include "oxtra/codegen/encoding/encoding.h"
#include <stdlib.h>

using namespace encoding;

TEST_CASE("instruction encoding is correct", "[encoding]") {
	SECTION("LUI") {
		FILE* assembly = fopen("lui.s", "w");
		fprintf(assembly, "lui t4, 28");
		fclose(assembly);

		utils::riscv_instruction_t lui_encoded_ = LUI(RiscVRegister::t4, 28);
		char* lui_encoded = reinterpret_cast<char*>(&lui_encoded_);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc lui.s -o lui -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary lui luidump");

		char lui_compiled[5] = {0};
		FILE* objdump = fopen("luidump", "r");
		fread(lui_compiled, 1, 4, objdump);
		fclose(objdump);

		REQUIRE(lui_compiled[0] == lui_encoded[0]);
		REQUIRE(lui_compiled[1] == lui_encoded[1]);
		REQUIRE(lui_compiled[2] == lui_encoded[2]);
		REQUIRE(lui_compiled[3] == lui_encoded[3]);
	}
}
