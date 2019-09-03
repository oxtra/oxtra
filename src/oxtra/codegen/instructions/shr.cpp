#include "shr.h"

void codegen::Shr::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRL(dst, dst, src);
}

bool codegen::Shr::end_of_block() const {
	return false;
}