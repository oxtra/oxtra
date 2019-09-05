#include "inc.h"

void codegen::Inc::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::ADDI(dst, dst, 1);
	update_overflow(batch, jump_table::Entry::unsupported_overflow, encoding::RiscVRegister::zero,
					encoding::RiscVRegister::zero, encoding::RiscVRegister::t4);
}