#include "jns.h"
#include "oxtra/codegen/helper.h"

//(SF == 0)
void codegen::Jns::generate(codegen::CodeBatch& batch) const {
	// load the sign-flag
	encoding::RiscVRegister flag = evaluate_sign(batch, encoding::RiscVRegister::t0);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	helper::append_eob(batch, get_address() + get_size());

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(flag, offset * 4);

	// compute the resulting operand
	translate_operand(batch, 0, helper::address_destination, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
	helper::append_eob(batch, helper::address_destination);
}