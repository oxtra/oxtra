#include "neg.h"

void codegen::Neg::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::NEG(dst, dst);
	update_overflow_unsupported(batch, "NEG", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "NEG", encoding::RiscVRegister::t4);
}