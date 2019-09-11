#include "scas.h"
#include "oxtra/dispatcher/dispatcher.h"

void codegen::Scas::generate(codegen::CodeBatch& batch) const {
	static constexpr auto rax = helper::map_reg(fadec::Register::rax);
	helper::load_from_register(batch, rax, get_operand_size(), dst_reg, false, true, false);

	RepeatableFlag::generate(batch);
}

void codegen::Scas::execute_operation(codegen::CodeBatch& batch) const {
	static constexpr auto
			rdi = helper::map_reg(fadec::Register::rdi);

	switch (get_operand_size()) {
		case 8:
			batch += encoding::LD(src_reg, rdi, 0);
			break;
		case 4:
			batch += encoding::LWU(src_reg, rdi, 0);
			break;
		case 2:
			batch += encoding::LHU(src_reg, rdi, 0);
			break;
		case 1:
			batch += encoding::LBU(src_reg, rdi, 0);
			break;
	}

	batch += encoding::SUB(diff_reg, dst_reg, src_reg);
	batch += encoding::ADDI(rdi, rdi, get_operand_size());
}