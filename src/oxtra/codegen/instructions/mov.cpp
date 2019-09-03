#include "mov.h"

void codegen::Mov::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::ADD(dst, encoding::RiscVRegister::zero, src);
}

bool codegen::Mov::end_of_block() const {
	return false;
}