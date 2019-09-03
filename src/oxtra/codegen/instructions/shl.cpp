#include "shl.h"

void codegen::Shl::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SLL(dst, dst, src);
}

bool codegen::Shl::end_of_block() const {
	return false;
}