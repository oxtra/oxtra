#include "mov.h"
#include "oxtra/codegen/helper.h"

void codegen::Mov::generate(CodeBatch& batch) const {
	const auto& dst = get_operand(0);
	const auto& src = get_operand(1);

	if (dst.get_type() == fadec::OperandType::reg) {
		const auto dst_register = helper::map_reg(dst.get_register());

		// if the destination operand is a 8-byte register then we can move the source operand result directly into it
		if (dst.get_size() == 8) {
			translate_operand(batch, 1, dst_register, encoding::RiscVRegister::t0, encoding::RiscVRegister::t1);
			return;
		}

		/*
		 * if the destination operand is a 4-byte register then we can move the source operand result into it
		 * and clear the upper 32 bits (saves 1 instruction)
		 */
		else if (dst.get_size() == 4) {
			translate_operand(batch, 1, dst_register, encoding::RiscVRegister::t0, encoding::RiscVRegister::t1);
			batch += encoding::SLLI(dst_register, dst_register, 32);
			batch += encoding::SRLI(dst_register, dst_register, 32);
			return;
		}
	}

	// extract the source-operand
	auto source_operand = encoding::RiscVRegister::t0;
	if (src.get_type() == fadec::OperandType::reg && src.get_register_type() != fadec::RegisterType::gph) {
		source_operand = helper::map_reg(src.get_register());
	} else
		translate_operand(batch, 1, source_operand, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);

	// write the value to the destination-register
	translate_destination(batch, source_operand, encoding::RiscVRegister::zero,
						  encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
}