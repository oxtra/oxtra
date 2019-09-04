#include "ja.h"
#include "oxtra/codegen/helper.h"

void codegen::Ja::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	encoding::RiscVRegister flag = evalute_sign(batch, encoding::RiscVRegister::t4);

	// append a dummy-branch
	batch += encoding::NOP();

	// generate the code to leave
	helper::ap


	if (get_operand(0).get_type() == fadec::OperandType::imm) {
		helper::load_immediate(batch, get_immediate(), helper::address_destination);
		jump_table::jump_reroute_static(batch);
	} else {
		translate_operand(batch, 0, helper::address_destination,
						  encoding::RiscVRegister::t0, encoding::RiscVRegister::t1);
		jump_table::jump_reroute_dynamic(batch);
	}
}