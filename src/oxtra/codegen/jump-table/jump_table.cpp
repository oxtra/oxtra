#include "jump_table.h"
#include "oxtra/codegen/helper.h"

void codegen::jump_table::jump_syscall_handler(CodeBatch& batch) {
	batch += encoding::JALR(encoding::RiscVRegister::ra, helper::jump_table_address, jump_table::Entries::virtual_syscall * 4);
}

void codegen::jump_table::jump_reroute_static(CodeBatch& batch) {
	batch += encoding::JALR(encoding::RiscVRegister::ra, helper::jump_table_address, jump_table::Entries::reroute_static * 4);
}

void codegen::jump_table::jump_reroute_dynamic(CodeBatch& batch) {
	batch += encoding::JALR(encoding::RiscVRegister::ra, helper::jump_table_address, jump_table::Entries::reroute_dynamic * 4);
}

void codegen::jump_table::jump_debug_break(CodeBatch& batch) {
	batch += encoding::JALR(encoding::RiscVRegister::ra, helper::jump_table_address, jump_table::Entries::debug_callback * 4);
}

void codegen::jump_table::jump_table_offset(CodeBatch& batch, encoding::RiscVRegister offset) {
	batch += encoding::ADD(offset, helper::jump_table_address, offset);
	batch += encoding::JALR(encoding::RiscVRegister::ra, offset, 0);
}

void codegen::jump_table::jump_table_entry(CodeBatch& batch, uint16_t entry) {
	batch += encoding::JALR(encoding::RiscVRegister::ra, helper::jump_table_address, entry * 4);
}