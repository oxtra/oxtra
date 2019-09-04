#ifndef OXTRA_JUMP_TABLE_H
#define OXTRA_JUMP_TABLE_H

#include "oxtra/codegen/code_batch.h"
#include "oxtra/codegen/encoding/encoding.h"
#include "oxtra/codegen/instruction.h"

namespace codegen::jump_table {
	// define the jump-table object
	void table_address();

	// This jump-table-enum must correspond to the order of the assembly-jump-table
	enum class Entry : uint16_t {
		virtual_syscall = 0,
		reroute_static,
		reroute_dynamic,
		debug_callback,

		// the flag evaluation functions modify t4, t5 and t6
		carry_clear,
		carry_set,
		carry_add_8,
		carry_add_16,
		carry_add_32,
		carry_add_64,
		carry_adc_8,
		carry_adc_16,
		carry_adc_32,
		carry_adc_64,

		overflow_clear,
		overflow_set,
		overflow_add_8,
		overflow_add_16,
		overflow_add_32,
		overflow_add_64,
		overflow_adc_8,
		overflow_adc_16,
		overflow_adc_32,
		overflow_adc_64,
	};
	/**
	 * Generates a jump to the syscall handler.
	 */
	void jump_syscall_handler(CodeBatch& batch);

	/**
	 * Generates a jump to reroute_static.
	 */
	void jump_reroute_static(CodeBatch& batch);

	/**
	* Generates a jump to reroute_dynamic.
	*/
	void jump_reroute_dynamic(CodeBatch& batch);

	/**
	 * Generates a jump to the debug break routine.
	 */
	void jump_debug_break(CodeBatch& batch);

	/**
	 * Generates a jump into the jump table to the offset in the register.
	 * @param offset The register that will contain the offset. This register will be overwritten.
	 */
	void jump_table_offset(CodeBatch& batch, encoding::RiscVRegister offset);

	/**
	 * Generates a jump into the jump table to the designated index.
	 * @param jump_entry The index into the jump-table
	 */
	void jump_table_entry(CodeBatch& batch, Entry jump_entry);
}


#endif //OXTRA_JUMP_TABLE_H
