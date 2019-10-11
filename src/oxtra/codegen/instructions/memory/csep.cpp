#include "csep.h"
#include "oxtra/codegen/helper.h"

void codegen::CSep::generate(CodeBatch& batch) const {
	if (get_operand_size() == 8) {
		batch += encoding::SRAI(helper::map_reg(fadec::Register::rdx), helper::map_reg(fadec::Register::rax), 63);
	} else {
		// set all bits of t0 to one or zero depending on the sign
		batch += encoding::SLLI(encoding::RiscVRegister::t0, helper::map_reg(fadec::Register::rax), 64 - get_operand_size() * 8);
		batch += encoding::SRAI(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, 63);

		codegen::helper::move_to_register(batch, helper::map_reg(fadec::Register::rdx), encoding::RiscVRegister::t0,
										  get_operand_size(), encoding::RiscVRegister::t1, false);
	}
}