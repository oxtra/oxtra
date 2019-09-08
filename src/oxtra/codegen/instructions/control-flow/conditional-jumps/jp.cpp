#include "jp.h"
#include "oxtra/codegen/helper.h"

//(PF == 1)
void codegen::Jp::generate(codegen::CodeBatch& batch) const {
	// load the parity-flag
	evaluate_parity(batch, encoding::RiscVRegister::t0);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	helper::append_eob(batch, get_address() + get_size());

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	// compute the resulting operand
	if (get_operand(0).get_type() == fadec::OperandType::imm) {
		helper::append_eob(batch, get_immediate());
	} else {
		translate_operand(batch, 0, 0, helper::address_destination, encoding::RiscVRegister::t0, true, false, false);
		helper::append_eob(batch, helper::address_destination);
	}
}