#include "cmov.h"
#include "oxtra/codegen/helper.h"

void codegen::Cmov::generate(codegen::CodeBatch& batch) const {
	if (get_operand(0).get_size() == 4) {
		// the upper 32-bits are cleared even if the condition is not true
		const auto dst_reg = helper::map_reg(get_operand(0).get_register());
		batch += encoding::SRLI(dst_reg, dst_reg, 32);
		batch += encoding::SLLI(dst_reg, dst_reg, 32);
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
			read_from_memory(batch, 1, load, encoding::RiscVRegister::t1, false);

			helper::move_to_register(batch, dst_reg, load, 2, encoding::RiscVRegister::t1, false);
		} else /*if (get_operand(0).get_size() >= 4)*/ {
			read_from_memory(batch, 1, dst_reg, encoding::RiscVRegister::t0, false);
		}
	}
	else /*if (src.get_type() == fadec::OperandType::reg)*/ {
		helper::move_to_register(batch, dst_reg, helper::map_reg(src.get_register()), get_operand(0).get_size(), encoding::RiscVRegister::t0,
								 false);
	}
}