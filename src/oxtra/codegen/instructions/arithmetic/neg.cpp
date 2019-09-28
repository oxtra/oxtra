#include "neg.h"
#include "oxtra/codegen/helper.h"

void codegen::Neg::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	const auto [carry, overflow] = helper::calculate_entries(
			jump_table::Entry::carry_neg, jump_table::Entry::overflow_neg, get_operand(0).get_size());

	update_carry(batch, &carry, &dst, nullptr, encoding::RiscVRegister::t4);
	update_overflow(batch, &overflow, &dst, nullptr, encoding::RiscVRegister::t4);

	batch += encoding::NEG(dst, dst);
}