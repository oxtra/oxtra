#include "stc.h"

void codegen::Stc::generate(CodeBatch& batch) const {
	update_carry(batch, true, encoding::RiscVRegister::t4);
}