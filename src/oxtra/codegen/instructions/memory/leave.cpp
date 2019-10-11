#include "oxtra/codegen/instructions/memory/leave.h"
#include "oxtra/codegen/helper.h"

void codegen::Leave::generate(CodeBatch& batch) const {
	batch += encoding::ADDI(helper::map_reg(fadec::Register::rsp), helper::map_reg(fadec::Register::rbp), 0);

	// Pop into RBP
	batch += encoding::LD(helper::map_reg(fadec::Register::rbp), helper::map_reg(fadec::Register::rsp), 0);
	batch += encoding::ADDI(helper::map_reg(fadec::Register::rsp), helper::map_reg(fadec::Register::rsp), 8);
}