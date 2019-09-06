#include "sub.h"

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SUB(dst, dst, src);
	update_overflow_unsupported(batch, "SUB", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SUB", encoding::RiscVRegister::t4);
}