#include "jump_table.h"
#include "oxtra/codegen/helper.h"

void codegen::jump_table::jump_syscall_handler(CodeBatch& batch) {
	jump_table_entry(batch, Entry::virtual_syscall);
}

void codegen::jump_table::jump_reroute_static(CodeBatch& batch) {
	jump_table_entry(batch, Entry::reroute_static);
}

void codegen::jump_table::jump_reroute_dynamic(CodeBatch& batch) {
	jump_table_entry(batch, Entry::reroute_dynamic);
}

void codegen::jump_table::jump_debugger(CodeBatch& batch) {
	jump_table_entry(batch, Entry::debug_callback);
}

void codegen::jump_table::jump_table_offset(CodeBatch& batch, encoding::RiscVRegister offset) {
	batch += encoding::ADD(offset, helper::jump_table_address, offset);
	batch += encoding::JALR(encoding::RiscVRegister::ra, offset, 0);
}

void codegen::jump_table::jump_table_entry(CodeBatch& batch, Entry entry) {
	batch += encoding::JALR(encoding::RiscVRegister::ra, helper::jump_table_address,
			static_cast<uint16_t>(entry) * 4);
}