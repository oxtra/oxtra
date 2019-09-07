#include "sub.h"
#include "oxtra/codegen/helper.h"

#include <spdlog/spdlog.h>

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	const auto [carry, overflow] = helper::calculate_entries(
			jump_table::Entry::carry_add, jump_table::Entry::overflow_sub, get_operand(0).get_size());

	update_carry_single(batch, carry, dst, encoding::RiscVRegister::t4);
	update_overflow(batch, overflow, src, dst, encoding::RiscVRegister::t4);

	batch += encoding::SUB(dst, dst, src);

	update_carry_single(batch, dst);
}

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	// pretty much the inverse of add with immediate

	const auto get_entries = [](intptr_t imm, uint8_t size) -> std::pair<jump_table::Entry, jump_table::Entry> {
		if (imm >= 0) {
			return helper::calculate_entries(
					jump_table::Entry::carry_add, jump_table::Entry::overflow_add_neg, size);
		} else {
			return helper::calculate_entries(
					jump_table::Entry::carry_add, jump_table::Entry::overflow_add_pos, size);
		}
	};

	const auto [carry, overflow] = get_entries(imm, get_operand(0).get_size());

	update_carry_single(batch, carry, dst, encoding::RiscVRegister::t4);
	update_overflow_single(batch, dst);

	if (imm == -0x800) {
		batch += encoding::ADDI(dst, dst, 1);
		batch += encoding::ADDI(dst, dst, -imm);
	} else
		batch += encoding::ADDI(dst, dst, -imm);

	update_carry_single(batch, dst);
	update_overflow_single(batch, overflow, dst, encoding::RiscVRegister::t4);
}