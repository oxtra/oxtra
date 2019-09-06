#include "dec.h"

void codegen::Dec::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::ADDI(dst, dst, -1);
	update_overflow_unsupported(batch, "DEC", encoding::RiscVRegister::t4);
}