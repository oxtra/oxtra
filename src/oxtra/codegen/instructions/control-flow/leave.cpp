#include "leave.h"
#include "oxtra/codegen/helper.h"

void codegen::Leave::generate(CodeBatch& batch) const {
	batch += encoding::ADDI(encoding::RiscVRegister::rsp, encoding::RiscVRegister::rbp, 0);

	// Pop into RBP
	batch += encoding::LD(encoding::RiscVRegister::rbp, encoding::RiscVRegister::rsp, 0);
	batch += encoding::ADDI(encoding::RiscVRegister::rsp, encoding::RiscVRegister::rsp, 8);
}