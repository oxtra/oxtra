#include "neg.h"

void codegen::Neg::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::NEG(dst, dst);
}