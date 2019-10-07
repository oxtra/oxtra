#include "call.h"

#include "oxtra/codegen/helper.h"
#include "oxtra/dispatcher/execution_context.h"
#include "oxtra/dispatcher/dispatcher.h"

void codegen::Call::generate(CodeBatch& batch) const {
	auto execution_context = dispatcher::Dispatcher::execution_context();
	auto&& call_table = execution_context->codegen->_call_table;

	// calculate the return address that is pushed onto the stack
	const auto return_address = get_address() + get_size();

	// offset of this call in the call table
	const auto table_offset = call_table.size() * sizeof(CallEntry);

	// add an entry to the call table
	call_table.emplace_back(return_address, 0ul);

	// update the call table address in case an allocation changed the address
	execution_context->guest.map.call_table = reinterpret_cast<uintptr_t>(call_table.data());

	// load the call table offset
	helper::load_immediate(batch, table_offset, encoding::RiscVRegister::t0);

	// push the call table offset onto the return stack
	batch += encoding::SW(helper::return_stack, encoding::RiscVRegister::t0, 0);
	batch += encoding::ADDI(helper::return_stack, helper::return_stack, 4);
	batch += encoding::ANDI(helper::return_stack, helper::return_stack, 0b1011'1111'1111);

	// get the return address from the call table
	if (table_offset < 0x800) {
		batch += encoding::LD(encoding::RiscVRegister::t1, helper::call_table_address, static_cast<uint16_t>(table_offset));
	} else {
		batch += encoding::ADD(encoding::RiscVRegister::t1, helper::call_table_address, encoding::RiscVRegister::t0);
		batch += encoding::LD(encoding::RiscVRegister::t1, encoding::RiscVRegister::t1, 0);
	}

	// push it onto the stack
	batch += encoding::ADDI(helper::map_reg(fadec::Register::rsp), helper::map_reg(fadec::Register::rsp), -8);
	batch += encoding::SD(helper::map_reg(fadec::Register::rsp), encoding::RiscVRegister::t1, 0);

	// the rest of the function behaves just like jump
	Jmp::generate(batch);
}