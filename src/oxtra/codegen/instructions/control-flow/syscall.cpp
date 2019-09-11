#include "syscall.h"
#include "oxtra/codegen/helper.h"

void codegen::Syscall::generate(CodeBatch& batch) const {
	jump_table::jump_syscall_handler(batch);
	helper::append_eob(batch, get_address() + get_size());
}