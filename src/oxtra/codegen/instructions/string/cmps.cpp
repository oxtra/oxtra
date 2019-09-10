#include "cmps.h"
#include "oxtra/dispatcher/dispatcher.h"

void codegen::Cmps::execute_operation(codegen::CodeBatch& batch) const {
	static constexpr auto
			rsi = helper::map_reg(fadec::Register::rsi),
			rdi = helper::map_reg(fadec::Register::rdi);

	switch (get_operand_size()) {
		case 8:
			batch += encoding::LD(src_reg, rsi, 0);
			batch += encoding::LD(dst_reg, rdi, 0);
			break;
		case 4:
			batch += encoding::LWU(src_reg, rsi, 0);
			batch += encoding::LWU(dst_reg, rdi, 0);
			break;
		case 2:
			batch += encoding::LHU(src_reg, rsi, 0);
			batch += encoding::LHU(dst_reg, rdi, 0);
			break;
		case 1:
			batch += encoding::LBU(src_reg, rsi, 0);
			batch += encoding::LBU(dst_reg, rdi, 0);
			break;
	}

	batch += encoding::SUB(diff_reg, src_reg, dst_reg);
	batch += encoding::ADDI(rsi, rsi, get_operand_size());
	batch += encoding::ADDI(rdi, rdi, get_operand_size());
}