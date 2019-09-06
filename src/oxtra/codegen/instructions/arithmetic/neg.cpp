#include "neg.h"

void codegen::Neg::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const {
	batch += encoding::NEG(dst, dst);
	update_overflow(batch, jump_table::Entry::unsupported_overflow, encoding::RiscVRegister::zero,
					encoding::RiscVRegister::zero, encoding::RiscVRegister::t4);
	update_carry(batch, jump_table::Entry::unsupported_carry, encoding::RiscVRegister::zero,
				 encoding::RiscVRegister::zero, encoding::RiscVRegister::t4);
}