#include "shr.h"

void codegen::Shr::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRL(dst, dst, src);
}