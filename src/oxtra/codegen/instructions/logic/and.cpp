#include "and.h"

void codegen::And::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::AND(dst, dst, src);
}