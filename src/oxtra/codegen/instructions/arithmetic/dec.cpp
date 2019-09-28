#include "dec.h"
#include "oxtra/codegen/helper.h"

void codegen::Dec::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	const auto overflow = helper::calculate_entries(
			jump_table::Entry::carry_clear, jump_table::Entry::overflow_dec, get_operand(0).get_size()).second;

	update_overflow_single(batch, overflow, dst, encoding::RiscVRegister::t4, true);
	batch += encoding::ADDI(dst, dst, -1);
}