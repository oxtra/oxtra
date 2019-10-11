#include "cex.h"
#include "oxtra/codegen/helper.h"

void codegen::Cex::generate(CodeBatch& batch) const {
	if (get_operand_size() == 8) {
		codegen::helper::sign_extend_register(batch, helper::map_reg(fadec::Register::rax), helper::map_reg(fadec::Register::rax), 4);
	} else {
		codegen::helper::sign_extend_register(batch, encoding::RiscVRegister::t0, helper::map_reg(fadec::Register::rax),
											  get_operand_size() / 2);
		codegen::helper::move_to_register(batch, helper::map_reg(fadec::Register::rax), encoding::RiscVRegister::t0,
										  get_operand_size(), encoding::RiscVRegister::t1, false);
	}
}