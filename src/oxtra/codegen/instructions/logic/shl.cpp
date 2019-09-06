#include "shl.h"

void codegen::Shl::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SLL(dst, dst, src);
	update_overflow_unsupported(batch, "SHL", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL", encoding::RiscVRegister::t4);
}