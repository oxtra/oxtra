#include "nop.h"

void codegen::Nop::generate(CodeBatch& batch) const {
	batch += encoding::ADDI(encoding::RiscVRegister::zero, encoding::RiscVRegister::zero, 0);
}