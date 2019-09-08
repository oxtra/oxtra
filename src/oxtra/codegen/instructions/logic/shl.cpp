#include "shl.h"

void codegen::Shl::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SLL(dst, dst, src);
	update_overflow_unsupported(batch, "SHL", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL", encoding::RiscVRegister::t4);
}

void codegen::Shl::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SLLI(dst, dst, imm);
	update_overflow_unsupported(batch, "SHL - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL - Imm", encoding::RiscVRegister::t4);
}