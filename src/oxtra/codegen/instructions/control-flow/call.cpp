#include "call.h"

#include "oxtra/codegen/helper.h"

void codegen::Call::generate(CodeBatch& batch) const {
	// add the instructions for pushing the ip
	helper::load_immediate(batch, get_address() + get_size(), encoding::RiscVRegister::t0);

	batch += encoding::ADDI(helper::map_reg(fadec::Register::rsp), helper::map_reg(fadec::Register::rsp), -8);
	batch += encoding::SD(helper::map_reg(fadec::Register::rsp), encoding::RiscVRegister::t0, 0);

	// the rest of the function behaves just like jump
	Jmp::generate(batch);
}