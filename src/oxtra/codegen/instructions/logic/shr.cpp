#include "shr.h"

void codegen::Shr::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRL(dst, dst, src);
	update_overflow_unsupported(batch, "SHR", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHR", encoding::RiscVRegister::t4);
}

void codegen::Shr::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SRLI(dst, dst, imm);
	update_overflow_unsupported(batch, "SHR - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHR - Imm", encoding::RiscVRegister::t4);
}