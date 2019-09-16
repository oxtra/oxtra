#include "clc.h"

void codegen::Clc::generate(CodeBatch& batch) const {
	update_carry(batch, false, encoding::RiscVRegister::t4);
}