#include "or.h"

void codegen::Or::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::OR(dst, dst, src);
	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);
}