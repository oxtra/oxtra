#include "mul.h"

void codegen::Mul::generate(codegen::CodeBatch& batch) const {

}
/*
void CodeGenerator::translate_mul(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	const auto is_unsigned = (inst.get_type() == fadec::InstructionType::MUL ||
							  inst.get_type() == fadec::InstructionType::MULX);

	const auto op_size = inst.get_operand(0).get_size();
	const auto mul_with_constant = inst.get_type() == fadec::InstructionType::IMUL3;

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