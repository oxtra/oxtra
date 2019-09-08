#include "inc.h"

void codegen::Inc::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::ADDI(dst, dst, 1);
	update_overflow_unsupported(batch, "INC", encoding::RiscVRegister::t4);
}