#include "movs.h"
#include "oxtra/codegen/helper.h"

void codegen::Movs::execute_operation(codegen::CodeBatch& batch) const {
	static constexpr auto
		rsi = helper::map_reg(fadec::Register::rsi),
		rdi = helper::map_reg(fadec::Register::rdi);

	switch (get_operand_size()) {
		case 8:
			batch += encoding::LD(encoding::RiscVRegister::t4, rsi, 0);
			batch += encoding::SD(rdi, encoding::RiscVRegister::t4, 0);
			break;
		case 4:
			batch += encoding::LWU(encoding::RiscVRegister::t4, rsi, 0);
			batch += encoding::SW(rdi, encoding::RiscVRegister::t4, 0);
			break;
		case 2:
			batch += encoding::LHU(encoding::RiscVRegister::t4, rsi, 0);
			batch += encoding::SH(rdi, encoding::RiscVRegister::t4, 0);
			break;
		case 1:
			batch += encoding::LBU(encoding::RiscVRegister::t4, rsi, 0);
			batch += encoding::SB(rdi, encoding::RiscVRegister::t4, 0);
			break;
	}

	batch += encoding::ADDI(rsi, rsi, get_operand_size());
	batch += encoding::ADDI(rdi, rdi, get_operand_size());
}