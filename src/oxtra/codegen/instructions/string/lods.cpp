#include "lods.h"
#include "oxtra/codegen/helper.h"

void codegen::Lods::execute_operation(codegen::CodeBatch& batch) const {
	static constexpr auto
			rax = helper::map_reg(fadec::Register::rax),
			rsi = helper::map_reg(fadec::Register::rsi);

	switch (get_operand_size()) {
		case 8:
			batch += encoding::LD(rax, rsi, 0);
			batch += encoding::ADDI(rsi, rsi, 8);
			return;
		case 4:
			batch += encoding::LWU(encoding::RiscVRegister::t4, rsi, 0);
			break;
		case 2:
			batch += encoding::LHU(encoding::RiscVRegister::t4, rsi, 0);
			break;
		case 1:
			batch += encoding::LBU(encoding::RiscVRegister::t4, rsi, 0);
			break;
	}

	helper::move_to_register(batch, rax, encoding::RiscVRegister::t4, get_operand_size(), encoding::RiscVRegister::t5, true);
	batch += encoding::ADDI(rsi, rsi, get_operand_size());
}