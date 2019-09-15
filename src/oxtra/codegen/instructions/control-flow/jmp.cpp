#include "jmp.h"
#include "oxtra/codegen/helper.h"

void codegen::Jmp::generate(codegen::CodeBatch& batch) const {
	if (get_operand(0).get_type() == fadec::OperandType::imm) {
		helper::append_eob(batch, get_immediate());
	} else {
		translate_operand(batch, 0, 0, helper::address_destination, encoding::RiscVRegister::t0, true, false, false, false);
		helper::append_eob(batch, helper::address_destination);
	}
}