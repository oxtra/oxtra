#include "jmp.h"
#include "oxtra/codegen/helper.h"

void codegen::Jmp::generate(codegen::CodeBatch& batch) const {
	if (get_operand(0).get_type() == fadec::OperandType::imm) {
		helper::append_eob(batch, get_immediate());
	} else {
		translate_operand(batch, get_operand(0), 0, helper::address_destination, encoding::RiscVRegister::t0, true, false, false, false);
		helper::append_eob(batch, helper::address_destination);
	}
}

uintptr_t codegen::Jmp::branch_address() const {
	if (get_operand(0).get_type() == fadec::OperandType::imm)
		return get_immediate();
	return 0;
}

uint8_t codegen::Jmp::control_flow_type() const {
	return 1;
}