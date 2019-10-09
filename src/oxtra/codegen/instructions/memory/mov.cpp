#include "mov.h"
#include "oxtra/codegen/helper.h"

void codegen::Mov::generate(CodeBatch& batch) const {
	const auto& dst = get_operand(0);

	if (dst.get_type() == fadec::OperandType::reg) {
		const auto dst_register = helper::map_reg(dst.get_register());
		/*
		 * If the destination operand is a 8-byte register then we can move the source operand result directly into it.
		 * If the destination operand is a 4-byte register then we can move the source operand result into it.
		 * and clear the upper 32 bits (saves 1 instruction)
		 */
		if (dst.get_size() >= 4) {
			const auto& src = get_operand(1);
			translate_operand(batch, src, nullptr, dst_register, encoding::RiscVRegister::t1, true, false, false, false);
			if (dst.get_size() == 4 && src.get_type() != fadec::OperandType::imm) {
				batch += encoding::SLLI(dst_register, dst_register, 32);
				batch += encoding::SRLI(dst_register, dst_register, 32);
			}
			return;
		}
	}

	// extract the source-operand
	auto source_operand = translate_operand(batch, get_operand(1), nullptr, encoding::RiscVRegister::t0, encoding::RiscVRegister::t2,
											false, false, false, false);

	// write the value to the destination-register
	translate_destination(batch, source_operand, encoding::RiscVRegister::zero,
						  encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
}