#include "sub.h"
#include "oxtra/codegen/helper.h"

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SUB(dst, dst, src);

	const auto [carry, overflow] = helper::calculate_entries(
			jump_table::Entry::carry_sub, jump_table::Entry::overflow_sub, get_operand(0).get_size());

	update_carry(batch, carry, src, dst, encoding::RiscVRegister::t4);
	update_overflow(batch, overflow, src, dst, encoding::RiscVRegister::t4);
}