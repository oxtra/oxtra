#include "jge.h"
#include "oxtra/codegen/helper.h"

//(SF == OF)
void codegen::Jge::generate(codegen::CodeBatch& batch) const {
	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t3);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	helper::append_eob(batch, get_address() + get_size());

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQ(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, offset * 4);

	// compute the resulting operand
	if (get_operand(0).get_type() == fadec::OperandType::imm) {
		helper::append_eob(batch, get_immediate());
	} else {
		translate_operand(batch, 0, 0, helper::address_destination, encoding::RiscVRegister::t0, true, false, false, false);
		helper::append_eob(batch, helper::address_destination);
	}
}