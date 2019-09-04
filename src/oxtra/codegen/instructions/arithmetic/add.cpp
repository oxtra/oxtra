#include "oxtra/codegen/instructions/arithmetic/add.h"
#include "oxtra/codegen/helper.h"

void codegen::Add::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::ADD(dst, src, dst);

	const auto [carry, overflow] = helper::calculate_entries(
			jump_table::Entry::carry_add, jump_table::Entry::overflow_add, get_operand(0).get_size());

	update_carry(batch, carry, src, dst, encoding::RiscVRegister::t4);
	update_overflow(batch, overflow, src, dst, encoding::RiscVRegister::t4);
}