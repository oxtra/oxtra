#include "stos.h"
#include "oxtra/codegen/helper.h"

void codegen::Stos::execute_operation(codegen::CodeBatch& batch) const {
	static constexpr auto rax = helper::map_reg(fadec::Register::rax);
	static constexpr auto rdi = helper::map_reg(fadec::Register::rdi);

	switch (get_operand_size()) {
		case 8:
			batch += encoding::SD(rdi, rax, 0);
			break;
		case 4:
			batch += encoding::SW(rdi, rax, 0);
			break;
		case 2:
			batch += encoding::SH(rdi, rax, 0);
			break;
		case 1:
			batch += encoding::SB(rdi, rax, 0);
			break;
	}

	batch += encoding::ADDI(rdi, rdi, get_operand_size());
}