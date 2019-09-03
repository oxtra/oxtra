#include "sar.h"

void codegen::Sar::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRA(dst, dst, src);
}

bool codegen::Sar::end_of_block() const {
	return false;
}