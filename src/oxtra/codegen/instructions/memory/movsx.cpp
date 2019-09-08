#include "movsx.h"
#include "oxtra/codegen/helper.h"

void codegen::Movsx::execute_operation(codegen::CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	helper::sign_extend_register(batch, dst, src, get_operand(1).get_size());
}