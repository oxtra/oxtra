#include "push.h"

#include "oxtra/codegen/helper.h"

void codegen::Push::generate(CodeBatch& batch) const {
	const auto& operand = get_operand(0);
	constexpr auto rsp_reg = helper::map_reg(fadec::Register::rsp);

	// update the stack-pointer
	batch += encoding::ADDI(rsp_reg, rsp_reg, -operand.get_size());

	// extract the value
	auto value_register = encoding::RiscVRegister::t0;
	if (operand.get_type() == fadec::OperandType::reg)
		value_register = helper::map_reg(operand.get_register());
	else
		translate_operand(batch, 0, value_register, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);

	// write the value to memory
	switch (operand.get_size()) {
		case 8:
			batch += encoding::SD(rsp_reg, value_register, 0);
			break;
		case 4:
			batch += encoding::SW(rsp_reg, value_register, 0);
			break;
		case 2:
			batch += encoding::SH(rsp_reg, value_register, 0);
			break;
	}
}