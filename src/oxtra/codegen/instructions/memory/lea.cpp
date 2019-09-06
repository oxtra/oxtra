#include "lea.h"
#include "oxtra/codegen/helper.h"

void codegen::Lea::generate(CodeBatch& batch) const {
	// extract the register
	auto dest = codegen::helper::map_reg(get_operand(0).get_register());

	// check if its a full register or half register operation
	if (get_operand(0).get_size() == 8) {
		auto lea = translate_memory(batch, 1, dest, encoding::RiscVRegister::t1);
		if (lea != dest)
			batch += encoding::MV(dest, lea);
	} else if (get_operand(0).get_size() == 4) {
		// translate_memory clears the upper 32 bit, if the address-size is 32bit. Otherwise we have to clear it
		auto lea = translate_memory(batch, 1, dest, encoding::RiscVRegister::t1);
		if (lea != dest)
			batch += encoding::MV(dest, lea);
		if (get_address_size() != 4) {
			batch += encoding::SLLI(dest, dest, 32);
			batch += encoding::SRLI(dest, dest, 32);
		}
	} else {
		// the register-encoding is a 16-bit register. Let move_to_register handle it
		auto lea = translate_memory(batch, 1, encoding::RiscVRegister::t0, encoding::RiscVRegister::t1);
		codegen::helper::move_to_register(batch, dest, lea,
										  codegen::helper::operand_to_register_access(get_operand(0)),
										  encoding::RiscVRegister::t2);
	}

}