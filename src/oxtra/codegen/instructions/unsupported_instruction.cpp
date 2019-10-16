#include "unsupported_instruction.h"

#include "oxtra/dispatcher/dispatcher.h"

void codegen::UnsupportedInstruction::generate(CodeBatch& batch) const {
	helper::load_immediate(batch, get_address(), encoding::RiscVRegister::a0);
	jump_table::jump_unsupported_instruction(batch);
}

void codegen::UnsupportedInstruction::handle_unsupported_instruction(utils::guest_addr_t addr) {
	fadec::Instruction inst{};
	if (fadec::decode(reinterpret_cast<uint8_t*>(addr), 0x10, fadec::DecodeMode::decode_64, addr,
					  inst) <= 0) {
		dispatcher::Dispatcher::fault_exit("failed to decode the unsupported instruction");
	}

	char format[256];
	fadec::format(inst, format, sizeof(format));

	char buffer[512];
	if (snprintf(buffer, sizeof(buffer), "failed to translate the instruction at 0x%lx: %s", addr, format) < 0) {
		dispatcher::Dispatcher::fault_exit("failed to format the string in unsupported instruction");
	}

	dispatcher::Dispatcher::fault_exit(buffer);
}