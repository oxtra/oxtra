#ifndef OXTRA_JUMP_TABLE_H
#define OXTRA_JUMP_TABLE_H

//#include "encoding/encoding.h"
//#include "../utils/types.h"
#include "codegen.h"

namespace codegen::jump_table {
	/**
	 * Generates a jump to the syscall handler.
	 * @param return_address The register that will contain the return address.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void jump_syscall_handler(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count) {
		riscv[count++] = encoding::JALR(return_address, CodeGenerator::jump_table_address, 0);
	}

	/**
	 * Generates a jump to reroute_static.
	 * @param return_address The register that will contain the return address.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	 void jump_reroute_static(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count) {
		riscv[count++] = encoding::JALR(return_address, CodeGenerator::jump_table_address, 4);
	 }

	/**
	* Generates a jump to reroute_dynamic.
	* @param return_address The register that will contain the return address.
	* @param riscv The pointer to the generated riscv instructions.
	* @param count The current length of the riscv instructions (i.e. the index of the next free position).
	*/
	void jump_reroute_dynamic(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count) {
		riscv[count++] = encoding::JALR(return_address, CodeGenerator::jump_table_address, 8);
	}
}

#endif //OXTRA_JUMP_TABLE_H