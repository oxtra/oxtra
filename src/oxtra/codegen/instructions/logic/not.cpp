#include "not.h"

void codegen::Not::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::NOT(dst, dst);
}