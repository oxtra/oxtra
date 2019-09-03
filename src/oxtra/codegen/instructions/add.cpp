#include "add.h"

void codegen::Add::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::ADD(dst, src, dst);
}

bool codegen::Add::end_of_block() const {
	return false;
}