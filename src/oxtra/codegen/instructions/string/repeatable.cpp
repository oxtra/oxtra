#include "repeatable.h"
#include "oxtra/dispatcher/dispatcher.h"

void codegen::Repeatable::generate(codegen::CodeBatch& batch) const {
	if (get_address_size() != 8)
		dispatcher::Dispatcher::fault_exit("Repeat prefix with address size < 8 not supported.");

	if (has_rep()) {
		static constexpr auto rcx = helper::map_reg(fadec::Register::rcx);
		const auto zero_test = batch.add(encoding::NOP());
		const auto loop_begin = batch.add(encoding::ADDI(rcx, rcx, -1));

		execute_operation(batch);

		batch += encoding::BNQZ(rcx, (loop_begin - batch.size()) * sizeof(utils::riscv_instruction_t));
		batch[zero_test] = encoding::BEQZ(rcx, (batch.size() - zero_test) * sizeof(utils::riscv_instruction_t));
	} else {
		execute_operation(batch);
	}
}

void codegen::RepeatableFlag::generate(codegen::CodeBatch& batch) const {
	if (get_address_size() != 8)
		dispatcher::Dispatcher::fault_exit("Repeat prefix with address size < 8 not supported.");

	if (has_rep()) { /* repz */
		generate_loop(batch, true);
	} else if (has_repnz()) { /* repnz */
		generate_loop(batch, false);
	} else {
		// execute the operation just once and update the flags unconditionally
		execute_operation(batch);
		update_flags(batch);
	}
}

void codegen::RepeatableFlag::generate_loop(codegen::CodeBatch& batch, bool z) const {
	static constexpr auto rcx = helper::map_reg(fadec::Register::rcx);

	// we test for zero before the loop
	const auto zero_test = batch.add(encoding::NOP());

	// decrement the loop counter
	const auto loop_begin = batch.add(encoding::ADDI(rcx, rcx, -1));

	// execute the string instruction
	execute_operation(batch);

	// this is where we test for z/nz
	const auto diff_test = batch.add(encoding::NOP());

	// do the loop again if the counter isn't 0
	batch += (encoding::BNQZ(rcx, (loop_begin - batch.size()) * sizeof(utils::riscv_instruction_t)));

	// replace the dummy with the actual branch instruction
	batch[diff_test] = (z ? encoding::BNQZ : encoding::BEQZ)(diff_reg, (batch.size() - diff_test) * sizeof(utils::riscv_instruction_t));

	// update the flags
	update_flags(batch);

	// if we didn't execute the instruction a single time because of the loop counter then also skip the flags
	batch[zero_test] = encoding::BEQZ(rcx, (batch.size() - zero_test) * sizeof(utils::riscv_instruction_t));
}

void codegen::RepeatableFlag::update_flags(codegen::CodeBatch& batch) const {
	update_zero(batch, diff_reg, get_operand_size());
	update_sign(batch, diff_reg, get_operand_size(), encoding::RiscVRegister::t3);
	update_parity(batch, diff_reg);

	const auto[carry, overflow] = helper::calculate_entries(jump_table::Entry::carry_add, jump_table::Entry::overflow_sub,
															get_operand_size());

	update_carry(batch, carry, diff_reg, dst_reg, encoding::RiscVRegister::t3);
	update_overflow(batch, overflow, dst_reg, src_reg, encoding::RiscVRegister::t3);
}