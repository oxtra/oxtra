#include "syscall.h"
#include "oxtra/codegen/helper.h"

void codegen::Syscall::generate(CodeBatch& batch) const {
	jump_table::jump_syscall_handler(batch);
	helper::load_address(batch, get_address() + get_size(), helper::address_destination);
	jump_table::jump_reroute_static(batch);
}