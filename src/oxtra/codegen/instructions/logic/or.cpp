#include "or.h"

void codegen::Or::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::OR(dst, dst, src);
}