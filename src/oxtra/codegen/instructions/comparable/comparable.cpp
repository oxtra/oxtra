#include "comparable.h"

void codegen::Comparable::generate(CodeBatch& batch) const {
	using namespace encoding;
	using namespace fadec;

	// load the two operands
	const auto& dest = get_operand(0);
	const auto& src = get_operand(1);

	// load the source-register
	auto source_register = RiscVRegister::zero;
	if (src.get_type() != OperandType::imm ||
		(static_cast<intptr_t>(get_immediate()) < -0x800 || static_cast<intptr_t>(get_immediate()) >= 0x800)) {
		source_register = translate_operand(batch, 1, nullptr, RiscVRegister::t2, RiscVRegister::t3, false, false, false);
	}

	// load the destination-register
	auto dest_register = translate_operand(batch, 0, nullptr, RiscVRegister::t0, RiscVRegister::t1, dest.get_size() < 4, false,
										   false);

	// call the callback to apply the changes
	auto result = encoding::RiscVRegister::zero;
	if (source_register != RiscVRegister::zero) {
		result = execute_operation(batch, dest_register, source_register);
	} else {
		result = execute_operation(batch, dest_register, static_cast<intptr_t>(get_immediate()));
	}

	// update the flags
	update_zero(batch, result, dest.get_size());
	update_sign(batch, result, dest.get_size(), RiscVRegister::t3);
	update_parity(batch, result);
}