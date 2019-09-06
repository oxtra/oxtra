#include "oxtra/codegen/helper.h"
#include "mul.h"

using namespace encoding;
using namespace fadec;
using namespace codegen::helper;

/**
 * This moves (lazily) moves a register to another while clearing the destination register. The register will be
 * sign extended if specified. If it is a high register (e.g. gph), while shifting back, 8 bits are added to the shift.
 *
 * If the register is specified with an operand size of 64 bit, nothing will happen and the src register simply returned.
 */
static RiscVRegister load_register(codegen::CodeBatch& batch, RiscVRegister src, RiscVRegister dest,
								   uint8_t operand_size, bool high_register, bool sign_extend) {
	uint8_t shamt = 64 - operand_size * 8;
	if (high_register) shamt -= 8;

	if (shamt == 0) return src;
	else {
		batch += encoding::SLLI(dest, src, shamt);

		if (high_register) shamt += 8;

		batch += (sign_extend ? encoding::SRAI : encoding::SRLI)(dest, dest, shamt);
	}

	return dest;
}

/**
 * This implementation can handle MUL, IMUL, IMUL2, and IMUL3.
 * Two source operands (s1 and s2) are multiplied and stored in the correct destination register.
 */
void codegen::Mul::generate(codegen::CodeBatch& batch) const {
	const auto op_size = get_operand(0).get_size();

	const auto is_signed = get_type() != InstructionType::MUL;
	const auto has_upper_destination =
			get_type() == InstructionType::MUL || get_type() == InstructionType::IMUL;

	// the default values are for MUL and IMUL
	auto upper_destination = RiscVRegister::rdx;
	auto lower_destination = RiscVRegister::rax;

	auto src1 = RiscVRegister::t1;
	auto src2 = RiscVRegister::t2;

	if (get_type() == InstructionType::IMUL2) {
		lower_destination = map_reg(get_operand(0).get_register());

		src1 = load_operand(batch, 0, src1, RiscVRegister::t4, RiscVRegister::t5, true);
		src2 = load_operand(batch, 1, src2, RiscVRegister::t4, RiscVRegister::t5, true);
	} else if (get_type() == InstructionType::IMUL3) {
		lower_destination = map_reg(get_operand(0).get_register());

		src1 = load_operand(batch, 1, src1, RiscVRegister::t4, RiscVRegister::t5, true);
		src2 = load_operand(batch, 2, src2, RiscVRegister::t4, RiscVRegister::t5, true);
	} else { // MUL, or IMUL
		src1 = load_register(batch, RiscVRegister::rax, src1, op_size, false, is_signed);
		src2 = load_operand(batch, 0, src2, RiscVRegister::t4, RiscVRegister::t5, is_signed);
	}

	if (op_size == 8) {
		// if the first multiplication would override a source, we have to use a temporary register
		RiscVRegister upper_result = (upper_destination == src1 || upper_destination == src2)
									 ? RiscVRegister::t4 : upper_destination;
		if (has_upper_destination) {
			batch += (is_signed ? MULH : MULHU)(upper_result, src1, src2);
		}

		batch += MUL(lower_destination, src1, src2);

		if (has_upper_destination && upper_result != upper_destination) {
			batch += MV(upper_destination, upper_result);
		}
	} else {
		// in here we do not need MULH(U) because the result can be stored in 64 bit (hence MUL).
		const auto mul_result = RiscVRegister::t0;
		batch += MUL(mul_result, src1, src2);

		if (op_size == 1) {
			//batch, lower_destination, RiscVRegister::t0, codegen::RegisterAccess::WORD, );
			move_to_register(batch, lower_destination, mul_result,
							 RegisterAccess::WORD, RiscVRegister::t4, false);
		} else {
			move_to_register(batch, lower_destination, mul_result,
							 operand_to_register_access(op_size), RiscVRegister::t4, false);
			if (has_upper_destination) {
				batch += SRLI(mul_result, mul_result, op_size * 8);
				move_to_register(batch, upper_destination, mul_result,
								 operand_to_register_access(op_size), RiscVRegister::t4, false);
			}
		}
	}
}

RiscVRegister codegen::Mul::load_operand(codegen::CodeBatch& batch, size_t index,
										 RiscVRegister reg, RiscVRegister temp_a, RiscVRegister temp_b,
										 bool sign_extend) const {
	const auto& operand = get_operand(index);
	uint8_t shamt = 64 - operand.get_size() * 8;

	if (operand.get_type() == OperandType::reg) {
		const auto gph = operand.get_register_type() == RegisterType::gph;
		const auto destination_register = (gph ? map_reg_high : map_reg)(operand.get_register());

		return load_register(batch, destination_register, reg, operand.get_size(), gph, sign_extend);
	} else if (operand.get_type() == OperandType::mem) {
		temp_a = translate_memory(batch, index, temp_a, temp_b);
		switch (operand.get_size()) {
			case 8:
				batch += encoding::LD(reg, temp_a, 0);
				break;
			case 4:
				batch += encoding::LW(reg, temp_a, 0);
				break;
			case 2:
				batch += encoding::LH(reg, temp_a, 0);
				break;
			case 1:
				batch += encoding::LB(reg, temp_a, 0);
				break;
		}
		// memory instructions sign extend per default, so we undo it (if required)
		if (!sign_extend && operand.get_size() != 8) {
			batch += SLLI(reg, reg, shamt);
			batch += SRLI(reg, reg, shamt);
		}
	} else if (operand.get_type() == OperandType::imm) {
		load_immediate(batch, get_immediate(), reg);
		// Immediates are interpreted sign_extended by both riscv and fadec, so we do nothing in here
	}

	return reg;
}