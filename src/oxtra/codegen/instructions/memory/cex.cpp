#include "cex.h"
#include "oxtra/codegen/helper.h"

void codegen::Cex::generate(CodeBatch& batch) const {
	if (get_operand_size() == 8) {
		codegen::helper::sign_extend_register(batch, encoding::RiscVRegister::rax, encoding::RiscVRegister::rax, 4);
	} else {
		codegen::helper::sign_extend_register(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::rax,
											  get_operand_size() / 2);
		codegen::helper::move_to_register(batch, encoding::RiscVRegister::rax, encoding::RiscVRegister::t0,
										  codegen::helper::operand_to_register_access(get_operand_size()),
										  encoding::RiscVRegister::t1);
	}
}