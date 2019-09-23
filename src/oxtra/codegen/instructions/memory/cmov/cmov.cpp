#include "oxtra/codegen/helper.h"
#include "cmov.h"

void codegen::Cmov::generate(codegen::CodeBatch& batch) const {
	if (get_operand(0).get_size() == 4) {
		// the upper 32-bits are cleared even if the condition is not true
		const auto dst_reg = helper::map_reg(get_operand(0).get_register());
		batch += encoding::SLLI(dst_reg, dst_reg, 32);
		batch += encoding::SRLI(dst_reg, dst_reg, 32);
	}

	execute_operation(batch);
}

void codegen::Cmov::generate_move(CodeBatch& batch) const {
	const auto& dst = get_operand(0);
	const auto& src = get_operand(1);

	const auto dst_reg = helper::map_reg(dst.get_register());

	/*
	 * The only operand sizes possible are 2, 4 and 8.
	 * 4 clears the upper 32-bits, 2 does not clear the upper 48 bits.
	 */

	if (src.get_type() == fadec::OperandType::mem) {
		if (get_operand(0).get_size() == 2) {
			constexpr auto load = encoding::RiscVRegister::t0;
			read_from_memory(batch, src, load, encoding::RiscVRegister::t1, false);

			helper::move_to_register(batch, dst_reg, load, 2, encoding::RiscVRegister::t1, false);
		} else /*if (get_operand(0).get_size() >= 4)*/ {
			read_from_memory(batch, src, dst_reg, encoding::RiscVRegister::t0, false);
		}
	} else /*if (src.get_type() == fadec::OperandType::reg)*/ {
		helper::move_to_register(batch, dst_reg, helper::map_reg(src.get_register()), get_operand(0).get_size(),
								 encoding::RiscVRegister::t0,
								 false);
	}
}

// cf == 0 && zf == 0
void codegen::Cmova::execute_operation(CodeBatch& batch) const {
	evaluate_zero(batch, encoding::RiscVRegister::t4);
	const auto zero = batch.add(encoding::NOP());

	evaluate_carry(batch);
	const auto carry = batch.add(encoding::NOP());

	generate_move(batch);

	batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
								 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));

	batch[carry] = encoding::BNQZ(encoding::RiscVRegister::t4,
								  (batch.size() - carry) * sizeof(utils::riscv_instruction_t));
}

// cf == 1 || zf == 1
void codegen::Cmovbe::execute_operation(CodeBatch& batch) const {
	evaluate_zero(batch, encoding::RiscVRegister::t4);
	const auto zero = batch.add(encoding::NOP());

	evaluate_carry(batch);
	const auto carry = batch.add(encoding::NOP());

	batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
								 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));

	generate_move(batch);

	batch[carry] = encoding::BEQZ(encoding::RiscVRegister::t4,
								  (batch.size() - carry) * sizeof(utils::riscv_instruction_t));
}

// cf == 1
void codegen::Cmovc::execute_operation(CodeBatch& batch) const {
	evaluate_carry(batch);
	const auto carry = batch.add(encoding::NOP());

	generate_move(batch);

	batch[carry] = encoding::BEQZ(encoding::RiscVRegister::t4,
								  (batch.size() - carry) * sizeof(utils::riscv_instruction_t));
}

// zf == 0 && sf == of
void codegen::Cmovg::execute_operation(CodeBatch& batch) const {
	evaluate_zero(batch, encoding::RiscVRegister::t4);
	const auto zero = batch.add(encoding::NOP());

	evaluate_sign(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::t5);

	evaluate_overflow(batch);
	const auto equal = batch.add(encoding::NOP());

	generate_move(batch);

	batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
								 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));

	batch[equal] = encoding::BNE(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4,
								 (batch.size() - equal) * sizeof(utils::riscv_instruction_t));
}

// sf == of
void codegen::Cmovge::execute_operation(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::t5);

	evaluate_overflow(batch);
	const auto equal = batch.add(encoding::NOP());

	generate_move(batch);

	batch[equal] = encoding::BNE(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4,
								 (batch.size() - equal) * sizeof(utils::riscv_instruction_t));
}

// sf != of
void codegen::Cmovl::execute_operation(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::t5);

	evaluate_overflow(batch);
	const auto equal = batch.add(encoding::NOP());

	generate_move(batch);

	batch[equal] = encoding::BEQ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4,
								 (batch.size() - equal) * sizeof(utils::riscv_instruction_t));
}

// zf == 1 || sf != of
void codegen::Cmovle::execute_operation(CodeBatch& batch) const {
	evaluate_zero(batch, encoding::RiscVRegister::t4);
	const auto zero = batch.add(encoding::NOP());

	evaluate_sign(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::t5);

	evaluate_overflow(batch);
	const auto equal = batch.add(encoding::NOP());

	batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
								 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));

	generate_move(batch);

	batch[equal] = encoding::BEQ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4,
								 (batch.size() - equal) * sizeof(utils::riscv_instruction_t));
}

// cf == 0
void codegen::Cmovnc::execute_operation(CodeBatch& batch) const {
	evaluate_carry(batch);
	const auto carry = batch.add(encoding::NOP());

	generate_move(batch);

	batch[carry] = encoding::BNQZ(encoding::RiscVRegister::t4,
								  (batch.size() - carry) * sizeof(utils::riscv_instruction_t));
}

// of == 0
void codegen::Cmovno::execute_operation(CodeBatch& batch) const {
	evaluate_overflow(batch);
	const auto overflow = batch.add(encoding::NOP());

	generate_move(batch);

	batch[overflow] = encoding::BNQZ(encoding::RiscVRegister::t4,
									 (batch.size() - overflow) * sizeof(utils::riscv_instruction_t));
}

// pf == 0
void codegen::Cmovnp::execute_operation(CodeBatch& batch) const {
	evaluate_parity(batch, encoding::RiscVRegister::t4, encoding::RiscVRegister::t5);
	const auto parity = batch.add(encoding::NOP());

	generate_move(batch);

	batch[parity] = encoding::BNQZ(encoding::RiscVRegister::t4,
								   (batch.size() - parity) * sizeof(utils::riscv_instruction_t));
}

// sf == 0
void codegen::Cmovns::execute_operation(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t4, encoding::RiscVRegister::t0);
	const auto sign = batch.add(encoding::NOP());

	generate_move(batch);

	batch[sign] = encoding::BNQZ(encoding::RiscVRegister::t4,
								 (batch.size() - sign) * sizeof(utils::riscv_instruction_t));
}

// zf == 0
void codegen::Cmovnz::execute_operation(CodeBatch& batch) const {
	evaluate_zero(batch, encoding::RiscVRegister::t4);
	const auto zero = batch.add(encoding::NOP());

	generate_move(batch);

	batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
								 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));
}

// of == 1
void codegen::Cmovo::execute_operation(CodeBatch& batch) const {
	evaluate_overflow(batch);
	const auto overflow = batch.add(encoding::NOP());

	generate_move(batch);

	batch[overflow] = encoding::BEQZ(encoding::RiscVRegister::t4,
									 (batch.size() - overflow) * sizeof(utils::riscv_instruction_t));
}

// pf == 1
void codegen::Cmovp::execute_operation(CodeBatch& batch) const {
	evaluate_parity(batch, encoding::RiscVRegister::t4, encoding::RiscVRegister::t5);
	const auto parity = batch.add(encoding::NOP());

	generate_move(batch);

	batch[parity] = encoding::BEQZ(encoding::RiscVRegister::t4,
								   (batch.size() - parity) * sizeof(utils::riscv_instruction_t));
}

// sf == 1
void codegen::Cmovs::execute_operation(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t4, encoding::RiscVRegister::t0);
	const auto sign = batch.add(encoding::NOP());

	generate_move(batch);

	batch[sign] = encoding::BEQZ(encoding::RiscVRegister::t4,
								 (batch.size() - sign) * sizeof(utils::riscv_instruction_t));
}

// zf == 1
void codegen::Cmovz::execute_operation(CodeBatch& batch) const {
	evaluate_zero(batch, encoding::RiscVRegister::t4);
	const auto zero = batch.add(encoding::NOP());

	generate_move(batch);

	batch[zero] = encoding::BEQZ(encoding::RiscVRegister::t4,
								 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));
}