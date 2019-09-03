#include "imul.h"

void codegen::Imul::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::MUL(dst, dst, src);
}