#ifndef OXTRA_JUMP_TABLE_H
#define OXTRA_JUMP_TABLE_H

#include "oxtra/codegen/Test/code_batch.h"
#include "oxtra/codegen/encoding/encoding.h"
#include "oxtra/codegen/Test/instruction.h"

namespace codegen::jump_table {
	// define the jump-table object
	extern uintptr_t table_address;

	// This jump-table-enum must correspond to the order of the assembly-jumptable
	struct Entries {
		static constexpr uint16_t virtual_syscall 	= 0;
		static constexpr uint16_t reroute_static 	= 1;
		static constexpr uint16_t reroute_dynamic 	= 2;
		static constexpr uint16_t debug_callback	= 3;
	};

	/**
	 * Generates a jump to the syscall handler.
	 * @param batch Store the current riscv-batch.
	 * @param return_address The register that will contain the return address.
	 */
	void jump_syscall_handler(CodeBatch& batch, encoding::RiscVRegister return_address);

	/**
	 * Generates a jump to reroute_static.
	 * @param batch Store the current riscv-batch.
	 * @param return_address The register that will contain the return address.
	 */
	void jump_reroute_static(CodeBatch& batch, encoding::RiscVRegister return_address);

	/**
	* Generates a jump to reroute_dynamic.
	* @param batch Store the current riscv-batch.
	* @param return_address The register that will contain the return address.
	*/
	void jump_reroute_dynamic(CodeBatch& batch, encoding::RiscVRegister return_address);

	/**
	 * Generates a jump into the jump table designated to flag evaluation.
	 * @param batch Store the current riscv-batch.
	 * @param offset The register that will contain the offset. This register will be overwritten.
	 * @param return_address The register that will contain the return address.
	 */
	void jump_flag_evaluation(CodeBatch& batch, encoding::RiscVRegister offset, encoding::RiscVRegister return_address);
}


#endif //OXTRA_JUMP_TABLE_H
