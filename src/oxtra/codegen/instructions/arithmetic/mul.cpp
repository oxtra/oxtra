#include "oxtra/codegen/helper.h"
#include "mul.h"

using namespace encoding;
using namespace fadec;
using namespace codegen::helper;

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

		src1 = translate_operand_lazy(batch, 0, src1, RiscVRegister::t4, RiscVRegister::t5);
		src2 = translate_operand_lazy(batch, 1, src2, RiscVRegister::t4, RiscVRegister::t5);
	} else if (get_type() == InstructionType::IMUL3) {
		lower_destination = map_reg(get_operand(0).get_register());

		src1 = translate_operand_lazy(batch, 1, src1, RiscVRegister::t4, RiscVRegister::t5);
		load_immediate(batch, get_immediate(), src2);
	} else { // MUL, or IMUL
		if (op_size == 8) {
			src1 = RiscVRegister::rax;
		} else {
			src1 = RiscVRegister::t1;
			move_to_register(batch, RiscVRegister::t1, RiscVRegister::rax, operand_to_register_access(op_size), RiscVRegister::t4, false);
		}
		src2 = translate_operand_lazy(batch, 0, src2, RiscVRegister::t4, RiscVRegister::t5);
	}

	if (op_size == 8) {
		// if the first multiplication would override a source, we have to use a temporary register
		RiscVRegister upper_result = (upper_destination == src1 || upper_destination == src2)
									 ? RiscVRegister::t4 : upper_destination;
		if (has_upper_destination) {
			batch += (is_signed ? MULH : MULHU)(upper_result, src1, src2);
		}

		batch += MUL(lower_destination, src1, src2);

		if (upper_result != upper_destination) {
			batch += MV(upper_destination, upper_result);
		}
	} else {
		// in here we do not need MULH(U) because the result can be stored in 64 bit (hence MUL).

		//if (is_signed) {
			//TODO: sign_extend? load specially?
		//}



		const auto mul_result = RiscVRegister::t0;
		batch += MUL(mul_result, src1, src2);

		if (op_size == 1) {
			//batch, lower_destination, RiscVRegister::t0, codegen::RegisterAccess::WORD, );
			move_to_register(batch, lower_destination, RiscVRegister::t0,
							 RegisterAccess::WORD, RiscVRegister::t4, false);
		} else {
			move_to_register(batch, lower_destination, RiscVRegister::t0,
							 operand_to_register_access(op_size), RiscVRegister::t4, false);
			batch += SRLI(mul_result, mul_result, op_size * 8);
			move_to_register(batch, upper_destination, RiscVRegister::t0,
							 operand_to_register_access(op_size), RiscVRegister::t4, false);
		}
	}
}

RiscVRegister codegen::Mul::translate_operand_lazy(codegen::CodeBatch& batch, size_t index,
												   RiscVRegister reg, RiscVRegister temp_a, RiscVRegister temp_b) const {
	if (get_operand(index).get_type() == OperandType::reg && get_operand(index).get_size() == 8) {
		return map_reg(get_operand(index).get_register());
	}

	translate_operand(batch, index, reg, temp_a, temp_b);

	return reg;
}
/*
void CodeGenerator::translate_mul(const Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	const auto is_unsigned = (inst.get_type() == InstructionType::MUL ||
							  inst.get_type() == InstructionType::MULX);

	const auto op_size = inst.get_operand(0).get_size();
	const auto mul_with_constant = inst.get_type() == InstructionType::IMUL3;

	const auto lower_destination = mul_with_constant ? map_reg(inst.get_operand(0).get_register()) : RiscVRegister::rax;
	constexpr auto upper_destination = RiscVRegister::rdx;

	constexpr auto operand_register = RiscVRegister::t0;
	const auto base_register = op_size == 8 ? lower_destination : RiscVRegister::t1;

	if (mul_with_constant) {
		load_unsigned_immediate(inst.get_immediate(), base_register, riscv, count);
		translate_operand(inst, 1, operand_register, RiscVRegister::t1, RiscVRegister::t2, riscv, count);
	} else {
		translate_operand(inst, 0, operand_register, RiscVRegister::t1, RiscVRegister::t2, riscv, count);
	}

	if (op_size == 8) {
		if (is_unsigned) {
			riscv[count++] = MULHU(upper_destination, base_register, operand_register);
		} else {
			riscv[count++] = MULH(upper_destination, base_register, operand_register);
		}
		riscv[count++] = MUL(lower_destination, base_register, operand_register);
	} else {
		// 32 bit multiplication only requires a single MUL command, since the result fits in a single 64 bit register
		const RegisterAccess register_type = operand_to_register_access(op_size);

		if (is_unsigned) {
			//since we use the base register as destination, we have to clear it first
			riscv[count++] = ADDI(base_register, RiscVRegister::zero, 0);
			move_to_register(base_register, lower_destination, register_type, RiscVRegister::t2, riscv, count, true);
		} else {
			sign_extend_register(operand_register, operand_register, op_size, riscv, count);

			if (!mul_with_constant) {
				sign_extend_register(base_register, lower_destination, op_size, riscv, count);
				riscv[count++] = MULH(RiscVRegister::t2, base_register, operand_register);
			}
		}

		riscv[count++] = MUL(base_register, base_register, operand_register);

		if (is_unsigned) {
			if (op_size == 1) {
				move_to_register(lower_destination, base_register, RegisterAccess::WORD, RiscVRegister::t2, riscv, count);
			} else {
				move_to_register(lower_destination, base_register, register_type, RiscVRegister::t2, riscv, count);
				riscv[count++] = SRLI(base_register, base_register, op_size * 8);
				move_to_register(upper_destination, base_register, register_type, RiscVRegister::t2, riscv, count);
			}
		} else {
			if (!mul_with_constant) {
				move_to_register(upper_destination, RiscVRegister::t2, register_type, RiscVRegister::t3, riscv, count);
			}
			move_to_register(lower_destination, base_register, register_type, RiscVRegister::t3, riscv, count);
		}
	}
}
 */