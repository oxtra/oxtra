#include "movzx.h"
#include "oxtra/codegen/helper.h"

void codegen::Movzx::generate(codegen::CodeBatch& batch) const {
	const auto& dst = get_operand(0);
	const auto& src = get_operand(1);

	const auto dst_reg = helper::map_reg(dst.get_register());

	if (src.get_type() == fadec::OperandType::reg) {
		const auto src_reg = helper::map_reg(src.get_register(), src.get_register_type());

		if (src.get_register_type() == fadec::RegisterType::gph) {
			batch += encoding::SLLI(dst_reg, src_reg, 48);
		} else {
			batch += encoding::SLLI(dst_reg, src_reg, 64 - (src.get_size() * 8));
		}

		batch += encoding::SRLI(dst_reg, dst_reg, 64 - (src.get_size() * 8));
	} else /*(src.get_type() == fadec::OperandType::mem)*/ {
		read_from_memory(batch, src, dst_reg, encoding::RiscVRegister::t0, false);
	}
}