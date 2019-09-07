#include "cmp.h"
#include "oxtra/codegen/helper.h"

encoding::RiscVRegister codegen::Cmp::execute_operation(codegen::CodeBatch& batch, encoding::RiscVRegister dst,
														encoding::RiscVRegister src) const {
	const auto [carry, overflow] = helper::calculate_entries(
			jump_table::Entry::carry_sub, jump_table::Entry::overflow_sub, get_operand(0).get_size());

	update_carry_single(batch, dst);
	update_overflow(batch, overflow, src, dst, encoding::RiscVRegister::t4);

	batch += encoding::SUB(encoding::RiscVRegister::t5, dst, src);

	update_carry_single(batch, carry, dst, encoding::RiscVRegister::t4);

	return encoding::RiscVRegister::t5;
}

encoding::RiscVRegister codegen::Cmp::execute_operation(codegen::CodeBatch& batch,
														encoding::RiscVRegister dst, intptr_t imm) const {
	if (imm == -0x800) {
		batch += encoding::ADDI(dst, dst, 1);
		batch += encoding::ADDI(dst, dst, -imm);
	} else
		batch += encoding::ADDI(dst, dst, -imm);
}