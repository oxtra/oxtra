#include "sub.h"
#include "oxtra/codegen/helper.h"

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	const auto [carry, overflow] = helper::calculate_entries(
			jump_table::Entry::carry_sub, jump_table::Entry::overflow_sub, get_operand(0).get_size());

	update_carry_single(batch, dst);
	update_overflow(batch, overflow, src, dst, encoding::RiscVRegister::t4);

	batch += encoding::SUB(dst, dst, src);

	update_carry_single(batch, carry, dst, encoding::RiscVRegister::t4);
}