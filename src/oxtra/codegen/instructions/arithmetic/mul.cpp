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
	const auto has_upper_destination = get_type() == InstructionType::MUL || get_type() == InstructionType::IMUL;

	// the default values are for MUL and IMUL
	auto upper_destination = helper::map_reg(fadec::Register::rdx);
	auto lower_destination = helper::map_reg(fadec::Register::rax);

	auto src1 = RiscVRegister::t1;
	auto src2 = RiscVRegister::t2;

	if (get_type() == InstructionType::IMUL2) {
		lower_destination = map_reg(get_operand(0).get_register()); // as of manual >= 16bit

		src1 = translate_operand(batch, get_operand(0), nullptr, src1, RiscVRegister::t4, false, true, true, false);
		src2 = translate_operand(batch, get_operand(1), nullptr, src2, RiscVRegister::t4, false, true, true, false);
	} else if (get_type() == InstructionType::IMUL3) {
		lower_destination = map_reg(get_operand(0).get_register()); // as of manual >= 16bit

		src1 = translate_operand(batch, get_operand(1), nullptr, src1, RiscVRegister::t4, false, true, true, false);
		src2 = translate_operand(batch, get_operand(2), nullptr, src2, RiscVRegister::t4, false, true, true, false);
	} else { // MUL, or IMUL
		src1 = helper::load_from_register(batch, helper::map_reg(fadec::Register::rax), op_size, RiscVRegister::t4, false, true, is_signed);
		src2 = translate_operand(batch, get_operand(0), nullptr, src2, RiscVRegister::t4, false, true, is_signed, false);
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
			// if there is no upper destination, we have (I)MUL and store the result in AX
			// otherwise, we have to store it in the first operand.
			move_to_register(batch, lower_destination, mul_result, RegisterAccess::WORD, RiscVRegister::t4, false);
		} else {
			move_to_register(batch, lower_destination, mul_result, op_size, RiscVRegister::t4, false);

			if (has_upper_destination) {
				if (op_size == 4) { // 32bit multiplication clears the upper bits, so we can omit the expensive moving
					batch += SRLI(upper_destination, mul_result, 32);
				} else {
					batch += SRLI(mul_result, mul_result, op_size * 8);
					move_to_register(batch, upper_destination, mul_result, op_size, RiscVRegister::t4, false);
				}
			}
		}
	}
	update_overflow_unsupported(batch, "MUL", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "MUL", encoding::RiscVRegister::t4);

}