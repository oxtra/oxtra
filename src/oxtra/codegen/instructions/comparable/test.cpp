#include "test.h"
#include "oxtra/codegen/helper.h"

encoding::RiscVRegister codegen::Test::execute_operation(codegen::CodeBatch& batch, encoding::RiscVRegister dst,
														 encoding::RiscVRegister src) const {
	batch += encoding::AND(encoding::RiscVRegister::t5, dst, src);

	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);

	return encoding::RiscVRegister::t5;
}

encoding::RiscVRegister codegen::Test::execute_operation(codegen::CodeBatch& batch,
														 encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::ANDI(encoding::RiscVRegister::t5, dst, imm);
	update_carry(batch, false, encoding::RiscVRegister::t4);
	update_overflow(batch, false, encoding::RiscVRegister::t4);
	return encoding::RiscVRegister::t5;
}