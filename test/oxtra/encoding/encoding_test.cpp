#include "../../catch2/catch.hpp"
#include "oxtra/codegen/encoding/encoding.h"
#include <stdlib.h>

using namespace encoding;

TEST_CASE("instruction encoding is correct", "[encoding]") {
	SECTION("LUI") {
		FILE* assembly = fopen("lui.s", "w");
		fprintf(assembly, "lui t4, 2874");
		fclose(assembly);

		utils::riscv_instruction_t lui_encoded = LUI(RiscVRegister::t4, 2874);

		system("/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc lui.s -o lui -nostdlib");
		system("objcopy -I elf64-little -j .text -O binary lui tmp");

		char lui_compiled[5];
		FILE* objdump = fopen("luidump", "r");
		fgets(lui_compiled, 4, objdump);
		fclose(objdump);

		REQUIRE(lui_compiled[0] == (char)(lui_encoded >> 24));
		REQUIRE(lui_compiled[1] == (char)(lui_encoded >> 16));
		REQUIRE(lui_compiled[2] == (char)(lui_encoded >> 8));
		REQUIRE(lui_compiled[3] == (char)(lui_encoded));
	}
}
