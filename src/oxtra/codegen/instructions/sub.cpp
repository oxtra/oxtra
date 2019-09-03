#include "sub.h"

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SUB(dst, dst, src);
}

bool codegen::Sub::end_of_block() const {
	return false;
}