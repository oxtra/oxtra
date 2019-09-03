#include "jump_table.h"
#include "oxtra/codegen/helper.h"

void codegen::jump_table::jump_syscall_handler(CodeBatch& batch, encoding::RiscVRegister return_address) {
	batch += encoding::JALR(return_address, helper::jump_table_address, 0);
}

void codegen::jump_table::jump_reroute_static(CodeBatch& batch, encoding::RiscVRegister return_address) {
	batch += encoding::JALR(return_address, helper::jump_table_address, 4);
}

void codegen::jump_table::jump_reroute_dynamic(CodeBatch& batch, encoding::RiscVRegister return_address) {
	batch += encoding::JALR(return_address, helper::jump_table_address, 8);
}

void codegen::jump_table::jump_debug_break(CodeBatch& batch, encoding::RiscVRegister return_address) {
	batch += encoding::JALR(return_address, helper::jump_table_address, 12);
}

void codegen::jump_table::jump_flag_evaluation(CodeBatch& batch, encoding::RiscVRegister offset, encoding::RiscVRegister return_address) {
	batch += encoding::ADD(offset, helper::jump_table_address, offset);
	batch += encoding::JALR(return_address, offset, 16);
}
