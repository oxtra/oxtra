#include "lea.h"
#include "oxtra/codegen/helper.h"

void codegen::Lea::generate(CodeBatch& batch) const {
	auto lea = translate_memory(batch, 1, encoding::RiscVRegister::t0, encoding::RiscVRegister::t1);
	codegen::helper::move_to_register(batch, codegen::helper::map_reg(get_operand(0).get_register()), lea,
									  codegen::helper::operand_to_register_access(get_operand(0).get_size()),
									  encoding::RiscVRegister::t2);
}