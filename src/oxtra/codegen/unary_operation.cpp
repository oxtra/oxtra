#include "unary_operation.h"
#include "oxtra/codegen/helper.h"

using namespace codegen;

codegen::UnaryOperation::UnaryOperation(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool eob)
	: codegen::Instruction{inst, update, require, eob} {}

void codegen::UnaryOperation::generate(codegen::CodeBatch& batch) const {
	using namespace encoding;
	using namespace fadec;

	const auto& dst = get_operand(0);

	// extract the register for the destination-value
	auto dest_register = RiscVRegister::t0;
	auto address = RiscVRegister::zero;
	if (dst.get_type() == OperandType::reg && dst.get_size() >= 4)
		dest_register = helper::map_reg(dst.get_register());
	else
		address = translate_operand(batch, 0, dest_register, RiscVRegister::t1, RiscVRegister::t2);

	// call the callback to apply the changes
	execute_operation(batch, dest_register);

	// write the value back to the destination
	translate_destination(batch, dest_register, address, RiscVRegister::t1, RiscVRegister::t2);
}