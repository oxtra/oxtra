#include "xor.h"

void codegen::Xor::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::XOR(dst, dst, src);
}