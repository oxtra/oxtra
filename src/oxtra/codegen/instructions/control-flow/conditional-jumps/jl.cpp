#include "jl.h"
#include "oxtra/codegen/helper.h"

//(SF != OF)
void codegen::Jl::generate(codegen::CodeBatch& batch) const {
	// load the carry flag
	evaluate_carry(batch);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	helper::append_eob(batch, get_address() + get_size());

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNE(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, offset * 4);

	// compute the resulting operand
	translate_operand(batch, 0, helper::address_destination, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
	helper::append_eob(batch, helper::address_destination);
}