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
	void jump_syscall_handler(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Generates a jump to reroute_static.
	 * @param return_address The register that will contain the return address.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void jump_reroute_static(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	* Generates a jump to reroute_dynamic.
	* @param return_address The register that will contain the return address.
	* @param riscv The pointer to the generated riscv instructions.
	* @param count The current length of the riscv instructions (i.e. the index of the next free position).
	*/
	void jump_reroute_dynamic(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Generates a jump into the jump table designated to flag evaluation.
	 * @param offset The register that will contain the offset. This register will be overwritten.
	 * @param return_address The register that will contain the return address.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void jump_flag_evaluation(encoding::RiscVRegister offset, encoding::RiscVRegister return_address,
						   utils::riscv_instruction_t* riscv, size_t& count);
}

#endif //OXTRA_JUMP_TABLE_H