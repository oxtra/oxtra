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

	// load the call table index from the return stack
	batch += encoding::ADDI(helper::return_stack, helper::return_stack, 0b0011'1111'1100);
	batch += encoding::ANDI(helper::return_stack, helper::return_stack, 0b1011'1111'1111);
	batch += encoding::LWU(encoding::RiscVRegister::t0, helper::return_stack, 0);

	// load the x86-return address from the call table
	batch += encoding::ADD(encoding::RiscVRegister::t1, helper::call_table_address, encoding::RiscVRegister::t0);
	batch += encoding::LD(encoding::RiscVRegister::t2, encoding::RiscVRegister::t1, 0);

	const auto modify = batch.add(encoding::NOP());

	// attach the rerouting
	helper::append_eob(batch, helper::address_destination);

	// jump over the reroute_dynamic if the return address was not modified
	batch.insert(modify, encoding::BEQ(helper::address_destination, encoding::RiscVRegister::t2,
									   batch.offset(modify, batch.size()) * sizeof(utils::riscv_instruction_t)));

	// load the riscv return address
	batch += encoding::LD(encoding::RiscVRegister::t3, encoding::RiscVRegister::t1, 8);

	const auto translate = batch.add(encoding::NOP());

	// jump to the riscv address
	batch += encoding::JALR(encoding::RiscVRegister::zero, encoding::RiscVRegister::t3, 0);

	// test if the return address is 0 (not yet translated)
	batch.insert(translate, encoding::BEQZ(encoding::RiscVRegister::t3,
			batch.offset(translate, batch.size()) * sizeof(utils::riscv_instruction_t)));

	// jump to reroute_return to translate the block and store the riscv address in the call table
	jump_table::jump_reroute_return(batch);
}