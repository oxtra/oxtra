#include "ja.h"
#include "oxtra/codegen/helper.h"

//(CF == 0 && ZF == 0)
void codegen::Ja::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	size_t leave = batch.size();
	helper::append_eob(batch, get_address() + get_size());

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	// load the carry flag
	evaluate_carry(batch);

	// generate the jump to the leave
	offset = batch.size() - leave;
	batch += encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	// compute the resulting operand
	translate_operand(batch, 0, helper::address_destination, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
	helper::append_eob(batch, helper::address_destination);
}