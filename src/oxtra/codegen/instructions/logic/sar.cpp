#include "sar.h"

void codegen::Sar::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRA(dst, dst, src);
	update_overflow_unsupported(batch, "SAR", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SAR", encoding::RiscVRegister::t4);
}

void codegen::Sar::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SRAI(dst, dst, imm);
	update_overflow_unsupported(batch, "SAR - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SAR - Imm", encoding::RiscVRegister::t4);
}