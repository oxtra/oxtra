#include "jcxz.h"
#include "oxtra/codegen/helper.h"

//(ECX == 0) / (RCX == 0)
void codegen::Jcxz::generate(codegen::CodeBatch& batch) const {
	// check what operand size is used
	auto reg = encoding::RiscVRegister::rcx;
	if (get_address_size() != 8){
		batch += encoding::SLLI(encoding::RiscVRegister::t0, reg, 32);
		reg = encoding::RiscVRegister::t0;
		batch += encoding::SRLI(reg, reg, 32);
	}

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	helper::append_eob(batch, get_address() + get_size());

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNQZ(reg, offset * 4);

	// compute the resulting operand
	translate_operand(batch, 0, helper::address_destination, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);
	helper::append_eob(batch, helper::address_destination);
}