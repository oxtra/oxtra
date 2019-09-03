#include "inc.h"

void codegen::Inc::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::ADDI(dst, dst, 1);
}