#include "neg.h"
#include "oxtra/codegen/helper.h"

void codegen::Neg::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	const auto [carry, overflow] = helper::calculate_entries(
			jump_table::Entry::carry_neg, jump_table::Entry::overflow_neg, get_operand(0).get_size());

	update_carry_single(batch, carry, dst, encoding::RiscVRegister::t4);
	update_overflow_single(batch, overflow, dst, encoding::RiscVRegister::t4);
	batch += encoding::NEG(dst, dst);
}