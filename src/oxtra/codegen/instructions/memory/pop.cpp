#include "pop.h"

#include "oxtra/codegen/helper.h"

void codegen::Pop::generate(CodeBatch& batch) const {
	constexpr auto rsp_reg = helper::map_reg(fadec::Register::rsp);
	const auto operand_size = get_operand(0).get_size();

	// if the destination operand is a register then we can optimize it
	if (get_operand(0).get_type() == fadec::OperandType::reg) {
		const auto dest_reg = helper::map_reg(get_operand(0).get_register());
		switch (operand_size) {
			case 8:
				batch += encoding::LD(dest_reg, rsp_reg, 0);
				break;
			case 4:
				batch += encoding::LWU(encoding::RiscVRegister::t0, rsp_reg, 0);
				move_to_register(batch, dest_reg, encoding::RiscVRegister::t0, helper::RegisterAccess::DWORD, encoding::RiscVRegister::t1, true);
				break;
			case 2:
				batch += encoding::LHU(encoding::RiscVRegister::t0, rsp_reg, 0);
				move_to_register(batch, dest_reg, encoding::RiscVRegister::t0, helper::RegisterAccess::WORD, encoding::RiscVRegister::t1, true);
				break;
		}
	} else if (get_operand(0).get_type() == fadec::OperandType::mem) {
		encoding::RiscVRegister reg = translate_memory(batch, 0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t1);
		switch (operand_size) {
			case 8:
				batch += encoding::LD(encoding::RiscVRegister::t1, rsp_reg, 0);
				batch += encoding::SD(reg, encoding::RiscVRegister::t1, 0);
				break;
			case 4:
				batch += encoding::LW(encoding::RiscVRegister::t1, rsp_reg, 0);
				batch += encoding::SW(reg, encoding::RiscVRegister::t1, 0);
				break;
			case 2:
				batch += encoding::LH(encoding::RiscVRegister::t1, rsp_reg, 0);
				batch += encoding::SH(reg, encoding::RiscVRegister::t1, 0);
				break;
		}
	}

	// update the stack pointer
	batch += encoding::ADDI(rsp_reg, rsp_reg, operand_size);
}