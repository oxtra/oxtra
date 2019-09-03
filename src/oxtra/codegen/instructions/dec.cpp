#include "dec.h"

void codegen::Dec::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::ADDI(dst, dst, -1);
}