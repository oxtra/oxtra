#include "sub.h"

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SUB(dst, dst, src);
	update_overflow_unsupported(batch, "SUB", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SUB", encoding::RiscVRegister::t4);
}

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	imm = -imm;
	if (imm == 0x800) {
		batch += encoding::ADDI(dst, dst, 1);
		batch += encoding::ADDI(dst, dst, imm);
	} else
		batch += encoding::ADDI(dst, dst, imm);
	update_carry_unsupported(batch, "SUB - Imm", encoding::RiscVRegister::t4);
	update_overflow_unsupported(batch, "SUB - Imm", encoding::RiscVRegister::t4);
}