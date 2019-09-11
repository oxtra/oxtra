#include "unary_operation.h"
#include "oxtra/codegen/helper.h"

using namespace codegen;

codegen::UnaryOperation::UnaryOperation(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool eob)
		: codegen::Instruction{inst, update, require, eob} {}

void codegen::UnaryOperation::generate(codegen::CodeBatch& batch) const {
	using namespace encoding;
	using namespace fadec;

	const auto& dest = get_operand(0);

	// extract the register for the destination-value
	auto address = RiscVRegister::zero;
	auto dest_register = translate_operand(batch, 0, &address, RiscVRegister::t0, RiscVRegister::t1, true, false, false);

	// call the callback to apply the changes
	execute_operation(batch, dest_register);

	// update the flags
	update_zero(batch, dest_register, dest.get_size());
	update_sign(batch, dest_register, dest.get_size(), RiscVRegister::t2);
	update_parity(batch, dest_register);

	// write the value back to the destination
	translate_destination(batch, dest_register, address, RiscVRegister::t2, RiscVRegister::t3);
}