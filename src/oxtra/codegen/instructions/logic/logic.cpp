#include "logic.h"

#include "oxtra/codegen/helper.h"

void codegen::And::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::AND(dst, dst, src);
	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);
}

void codegen::And::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::ANDI(dst, dst, imm);
	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);
}

void codegen::Nop::generate(CodeBatch& batch) const {
	unused_parameter(batch);
}

void codegen::Not::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::NOT(dst, dst);
}

void codegen::Or::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::OR(dst, dst, src);
	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);
}

void codegen::Or::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::ORI(dst, dst, imm);
	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);
}

void codegen::Rotation::generate(codegen::CodeBatch& batch) const {
	const auto& dst = get_operand(0);

	if (dst.get_type() == fadec::OperandType::mem) {
		const auto address = read_from_memory(batch, dst, encoding::RiscVRegister::t0, encoding::RiscVRegister::t6, false);

		const auto result = rotate(batch, encoding::RiscVRegister::t0);

		write_to_memory(batch, dst, result, encoding::RiscVRegister::t5, encoding::RiscVRegister::t6, address);
	} else /*if (dst.get_type() == fadec::OperandType::reg)*/ {
		if (dst.get_register_type() == fadec::RegisterType::gph) {
			const auto dst_reg = helper::map_reg_high(dst.get_register());
			// shift it into the lower byte
			batch += encoding::SRLI(encoding::RiscVRegister::t0, dst_reg, 8);

			const auto result = rotate(batch, encoding::RiscVRegister::t0);

			helper::move_to_register(batch, dst_reg, result, 0, encoding::RiscVRegister::t6, false);
		} else {
			const auto dst_reg = helper::map_reg(dst.get_register());

			const auto result = rotate(batch, dst_reg);

			helper::move_to_register(batch, dst_reg, result, dst.get_size(), encoding::RiscVRegister::t6, dst_reg == result);
		}
	}

	update_carry_unsupported(batch, "Rotation", encoding::RiscVRegister::t0);
	update_overflow_unsupported(batch, "Rotation", encoding::RiscVRegister::t0);
}

encoding::RiscVRegister codegen::RolImm::rotate(codegen::CodeBatch& batch, encoding::RiscVRegister reg) const {
	const auto& dst = get_operand(0);

	const auto size = dst.get_size() * 8;
	const auto rot_amount = get_immediate() % size;

	// store the bits that will be shifted out
	if (size == 32) {
		batch += encoding::SRLIW(encoding::RiscVRegister::t1, reg, size - rot_amount);
	} else {
		batch += encoding::SRLI(encoding::RiscVRegister::t1, reg, size - rot_amount);

		if (size < 32) {
			batch += encoding::SLLI(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, 64 - rot_amount);
			batch += encoding::SRLI(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, 64 - rot_amount);
		}
	}


	// shift the bits by the rotation amount
	batch += encoding::SLLI(encoding::RiscVRegister::t2, reg, rot_amount);

	if (size >= 32) {
		// put the bits that got shifted out into the lower bits
		batch += encoding::OR(reg, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
		return reg;
	} else {
		// put the bits that got shifted out into the lower bits
		batch += encoding::OR(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);

		return encoding::RiscVRegister::t1;
	}
}

encoding::RiscVRegister codegen::RolCl::rotate(codegen::CodeBatch& batch, encoding::RiscVRegister reg) const {
	const auto& dst = get_operand(0);
	const auto size = dst.get_size() * 8;

	constexpr auto cl = helper::map_reg(fadec::Register::cl);

	if (size >= 32) {
		// calculate the shift amount for the bits that will be shifted out
		batch += encoding::ADDI(encoding::RiscVRegister::t4, encoding::RiscVRegister::zero, size);
		batch += encoding::SUB(encoding::RiscVRegister::t4, encoding::RiscVRegister::t4, cl);

		if (size == 64) {
			// get the bits that would be shifted out
			batch += encoding::SRL(encoding::RiscVRegister::t2, reg, encoding::RiscVRegister::t4);

			// shift the bits by the rotation amount
			batch += encoding::SLL(encoding::RiscVRegister::t1, reg, cl);
		} else /*if (size == 32) */ {
			// get the bits that would be shifted out
			batch += encoding::SRLW(encoding::RiscVRegister::t2, reg, encoding::RiscVRegister::t4);

			// shift the bits by the rotation amount
			batch += encoding::SLLW(encoding::RiscVRegister::t1, reg, cl);
		}

		batch += encoding::OR(reg, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
		return reg;
	}

	//  clear the upper bits
	batch += encoding::SLLI(encoding::RiscVRegister::t1, reg, 64 - size);
	batch += encoding::SRLI(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, 64 - size);

	constexpr auto rot_amount = encoding::RiscVRegister::t3;
	// clamp the rotation amount
	batch += encoding::ANDI(rot_amount, cl, size - 1);

	// shift the bits by the rotation amount
	batch += encoding::SLL(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, rot_amount);

	// get the bits that got shifted out
	batch += encoding::SRLI(encoding::RiscVRegister::t2, encoding::RiscVRegister::t1, size);

	batch += encoding::OR(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);

	return encoding::RiscVRegister::t1;
}

encoding::RiscVRegister codegen::RorImm::rotate(codegen::CodeBatch& batch, encoding::RiscVRegister reg) const {
	const auto& dst = get_operand(0);

	const auto size = dst.get_size() * 8;
	const auto rot_amount = get_immediate() % size;

	if (size == 32) {
		batch += encoding::SLLIW(encoding::RiscVRegister::t1, reg, size - rot_amount);
		batch += encoding::SRLIW(encoding::RiscVRegister::t2, reg, rot_amount);
	} else {
		batch += encoding::SLLI(encoding::RiscVRegister::t1, reg, size - rot_amount);

		if (size != 64) {
			batch += encoding::SLLI(encoding::RiscVRegister::t2, reg, 64 - size);
			batch += encoding::SRLI(encoding::RiscVRegister::t2, encoding::RiscVRegister::t2, 64 - size + rot_amount);
		} else {
			batch += encoding::SRLI(encoding::RiscVRegister::t2, reg, rot_amount);
		}
	}

	if (size >= 32) {
		// put the bits that got shifted out into the lower bits
		batch += encoding::OR(reg, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
		return reg;
	} else {
		// put the bits that got shifted out into the lower bits
		batch += encoding::OR(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);

		return encoding::RiscVRegister::t1;
	}
}

encoding::RiscVRegister codegen::RorCl::rotate(codegen::CodeBatch& batch, encoding::RiscVRegister reg) const {
	const auto& dst = get_operand(0);
	const auto size = dst.get_size() * 8;

	constexpr auto cl = helper::map_reg(fadec::Register::cl);

	// calculate the shift amount for the bits that will be shifted out
	batch += encoding::ADDI(encoding::RiscVRegister::t4, encoding::RiscVRegister::zero, size);
	batch += encoding::SUB(encoding::RiscVRegister::t4, encoding::RiscVRegister::t4, cl);

	if (size == 32) {
		// get the bits that would be shifted out
		batch += encoding::SLLW(encoding::RiscVRegister::t2, reg, encoding::RiscVRegister::t4);

		// shift the bits by the rotation amount
		batch += encoding::SRLW(encoding::RiscVRegister::t1, reg, cl);
	} else if (size == 64) {
		// get the bits that would be shifted out
		batch += encoding::SLL(encoding::RiscVRegister::t2, reg, encoding::RiscVRegister::t4);

		// shift the bits by the rotation amount
		batch += encoding::SRL(encoding::RiscVRegister::t1, reg, cl);
	} else {
		// clear the upper bits
		batch += encoding::SLLI(encoding::RiscVRegister::t1, reg, 64 - size);
		batch += encoding::SRLI(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, 64 - size);

		// get the bits that would be shifted out
		batch += encoding::SLL(encoding::RiscVRegister::t2, encoding::RiscVRegister::t1, encoding::RiscVRegister::t4);

		// shift the bits by the rotation amount
		batch += encoding::SRL(encoding::RiscVRegister::t1, reg, cl);

		batch += encoding::OR(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
		return encoding::RiscVRegister::t1;
	}

	batch += encoding::OR(reg, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
	return reg;
}

void codegen::ShiftCl::generate(codegen::CodeBatch& batch) const {
	const auto& dst = get_operand(0);
	const auto operand_size = dst.get_size();
	if (dst.get_type() == fadec::OperandType::reg) {
		const auto dst_reg = helper::map_reg(dst.get_register(), dst.get_register_type());

		if (operand_size == 8) {
			execute_operation(batch, dst_reg, dst_reg);
		} else if (operand_size == 4) {
			execute_operation(batch, dst_reg, dst_reg);
			batch += encoding::SLLI(dst_reg, dst_reg, 32);
			batch += encoding::SRLI(dst_reg, dst_reg, 32);
		} else {
			constexpr auto temp = encoding::RiscVRegister::t0;
			if (dst.get_register_type() == fadec::RegisterType::gph) {
				batch += encoding::SRLI(temp, dst_reg, 8);
				execute_operation(batch, temp, temp);
				helper::move_to_register(batch, dst_reg, temp, 0, encoding::RiscVRegister::t1, false);
			} else {
				execute_operation(batch, temp, dst_reg);
				helper::move_to_register(batch, dst_reg, temp, operand_size, encoding::RiscVRegister::t1, false);
			}
		}
	} else /*if (dst.get_type() == fadec::OperandType::mem)*/ {
		constexpr auto temp = encoding::RiscVRegister::t0;
		const auto address = read_from_memory(batch, dst, temp, encoding::RiscVRegister::t1, false);
		execute_operation(batch, temp, temp);
		write_to_memory(batch, dst, temp, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2, address);
	}
}

void codegen::Sar::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRA(dst, dst, src);
	update_overflow_unsupported(batch, "SAR", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SAR", encoding::RiscVRegister::t4);
}

void codegen::Sar::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SRAI(dst, dst, imm);
	update_overflow_unsupported(batch, "SAR - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SAR - Imm", encoding::RiscVRegister::t4);
}

void codegen::SarCl::execute_operation(codegen::CodeBatch& batch,
									   encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	if (get_operand(0).get_size() == 8) {
		batch += encoding::SRA(dst, src, helper::map_reg(fadec::Register::cl));
	} else {
		batch += encoding::SRAW(dst, src, helper::map_reg(fadec::Register::cl));
	}

	update_overflow_unsupported(batch, "SAR - Cl", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SAR - Cl", encoding::RiscVRegister::t4);
}

void codegen::Shl::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SLL(dst, dst, src);
	update_overflow_unsupported(batch, "SHL", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL", encoding::RiscVRegister::t4);
}

void codegen::Shl::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SLLI(dst, dst, imm);
	update_overflow_unsupported(batch, "SHL - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL - Imm", encoding::RiscVRegister::t4);
}

void codegen::ShlCl::execute_operation(codegen::CodeBatch& batch,
									   encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	if (get_operand(0).get_size() == 8) {
		batch += encoding::SLL(dst, src, helper::map_reg(fadec::Register::cl));
	} else {
		batch += encoding::SLLW(dst, src, helper::map_reg(fadec::Register::cl));
	}

	update_overflow_unsupported(batch, "SHL - Cl", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL - Cl", encoding::RiscVRegister::t4);
}

void codegen::Shr::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRL(dst, dst, src);
	update_overflow_unsupported(batch, "SHR", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHR", encoding::RiscVRegister::t4);
}

void codegen::Shr::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SRLI(dst, dst, imm);
	update_overflow_unsupported(batch, "SHR - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHR - Imm", encoding::RiscVRegister::t4);
}

void codegen::ShrCl::execute_operation(codegen::CodeBatch& batch,
									   encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	if (get_operand(0).get_size() == 8) {
		batch += encoding::SRL(dst, src, helper::map_reg(fadec::Register::cl));
	} else {
		batch += encoding::SRLW(dst, src, helper::map_reg(fadec::Register::cl));
	}

	update_overflow_unsupported(batch, "SHR - Cl", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHR - Cl", encoding::RiscVRegister::t4);
}

void codegen::Xor::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::XOR(dst, dst, src);
	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);
}

void codegen::Xor::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::XORI(dst, dst, imm);
	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);
}