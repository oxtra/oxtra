#include "jmp.h"
#include "oxtra/codegen/helper.h"

void codegen::Jmp::generate(codegen::CodeBatch& batch) const {
	if (get_operand(0).get_type() == fadec::OperandType::imm) {
		helper::load_immediate(batch, get_immediate(), helper::address_destination);
		jump_table::jump_reroute_static(batch, encoding::RiscVRegister::ra);
	} else {
		translate_operand(batch, 0, helper::address_destination,
				encoding::RiscVRegister::t0, encoding::RiscVRegister::t1);
		jump_table::jump_reroute_dynamic(batch, encoding::RiscVRegister::ra);
	}
}