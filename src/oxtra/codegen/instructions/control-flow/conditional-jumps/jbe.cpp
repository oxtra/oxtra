#include "jbe.h"
#include "oxtra/codegen/helper.h"

//(CF == 1 || ZF == 1)
void codegen::Jbe::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	encoding::RiscVRegister flag = evaluate_zero(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to jump to the result
	size_t finish = batch.size();
	translate_operand(batch, 0, helper::address_destination, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
	helper::append_eob(batch, helper::address_destination);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(flag, offset * 4);

	// load the carry flag
	flag = evaluate_carry(batch);

	// generate the jump to the out-jump
	offset = batch.size() - finish;
	batch += encoding::BNQZ(flag, offset * 4);

	// generate the code to leave the function
	helper::append_eob(batch, get_address() + get_size());
}