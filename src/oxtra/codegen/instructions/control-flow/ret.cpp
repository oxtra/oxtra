#include "ret.h"
#include "oxtra/codegen/helper.h"

void codegen::Ret::generate(CodeBatch& batch) const {
	constexpr auto rsp_reg = helper::map_reg(fadec::Register::rsp);

	// pop the ip from the stack
	batch += encoding::LD(helper::address_destination, rsp_reg, 0);

	// pop the immediate from the stack
	if (get_immediate() + 8 < 0x800)
		batch += encoding::ADDI(rsp_reg, rsp_reg, get_immediate() + 8);
	else {
		helper::load_immediate(batch, get_immediate() + 8, encoding::RiscVRegister::t0);
		batch += encoding::ADD(rsp_reg, rsp_reg, encoding::RiscVRegister::t0);
	}

	// attach the rerouting
	jump_table::jump_reroute_dynamic(batch);
}