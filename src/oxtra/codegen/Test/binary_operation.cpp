#include "binary_operation.h"
#include "oxtra/codegen/helper.h"

using namespace codegen;

codegen::BinaryOperation::BinaryOperation(const fadec::Instruction& inst, uint8_t update, uint8_t require)
		: Instruction{inst, update, require, false} {}

void codegen::BinaryOperation::generate(CodeBatch& batch) const {
	using namespace encoding;
	using namespace fadec;

	const auto& dst = get_operand(0);
	const auto& src = get_operand(1);

	// extract the source-operand
	auto source_register = RiscVRegister::t0;
	if (src.get_type() == OperandType::reg && src.get_register_type() != RegisterType::gph)
	source_register = helper::map_reg(get_operand(1).get_register());
	else
	translate_operand(batch, 1, source_register, RiscVRegister::t2, RiscVRegister::t3);

	// extract the register for the destination-value
	auto dest_register = RiscVRegister::t1;
	auto address = RiscVRegister::zero;
	if (dst.get_type() == OperandType::reg && dst.get_size() >= 4)
	dest_register = helper::map_reg(dst.get_register());
	else
	address = translate_operand(batch, 0, dest_register, RiscVRegister::t2, RiscVRegister::t3);

	// call the callback to apply the changes
	execute_operation(batch, dest_register, source_register);

	// write the value back to the destination
	translate_destination(batch, dest_register, address, RiscVRegister::t2, RiscVRegister::t3);
}