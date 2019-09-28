#include "oxtra/codegen/instructions/arithmetic/add.h"
#include "oxtra/codegen/helper.h"

void codegen::Add::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	const auto [carry, overflow] = helper::calculate_entries(
			jump_table::Entry::carry_add, jump_table::Entry::overflow_add, get_operand(0).get_size());

	update_overflow(batch, &overflow, &src, &dst, encoding::RiscVRegister::t4);

	batch += encoding::ADD(dst, src, dst);

	update_carry(batch, &carry, &src, &dst, encoding::RiscVRegister::t4);
}

void codegen::Add::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	const auto get_entries = [](intptr_t imm, uint8_t size) -> std::pair<jump_table::Entry, jump_table::Entry> {
		if (imm >= 0) {
			return helper::calculate_entries(
					jump_table::Entry::carry_add, jump_table::Entry::overflow_add_pos, size);
		} else {
			return helper::calculate_entries(
					jump_table::Entry::carry_add, jump_table::Entry::overflow_add_neg, size);
		}
	};

	const auto [carry, overflow] = get_entries(imm, get_operand(0).get_size());

	update_carry(batch, &carry, &dst, nullptr, encoding::RiscVRegister::t4);
	update_overflow(batch, &overflow, &dst, nullptr, encoding::RiscVRegister::t4);
	batch += encoding::ADDI(dst, dst, imm);
	update_carry(batch, nullptr, nullptr, &dst);
	update_overflow(batch, nullptr, nullptr, &dst);
}