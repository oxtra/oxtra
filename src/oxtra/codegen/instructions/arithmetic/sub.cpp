#include "sub.h"

void codegen::Sub::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SUB(dst, dst, src);
	update_overflow(batch, jump_table::Entry::unsupported_overflow, encoding::RiscVRegister::zero,
					encoding::RiscVRegister::zero, encoding::RiscVRegister::t4);
	update_carry(batch, jump_table::Entry::unsupported_carry, encoding::RiscVRegister::zero,
				 encoding::RiscVRegister::zero, encoding::RiscVRegister::t4);
}