#include <oxtra/dispatcher/dispatcher.h>
#include "binary_operation.h"
#include "oxtra/codegen/helper.h"

using namespace codegen;

codegen::BinaryOperation::BinaryOperation(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool use_dest)
		: Instruction{inst, update, require, false}, dest_value{use_dest} {}

void codegen::BinaryOperation::generate(CodeBatch& batch) const {
	using namespace encoding;
	using namespace fadec;

	// load the two operands
	const auto& dest = get_operand(0);
	const auto& src = get_operand(1);

	// load the source-register
	auto source_register = RiscVRegister::zero;
	if (src.get_type() != OperandType::imm ||
		(static_cast<intptr_t>(get_immediate()) < -0x800 || static_cast<intptr_t>(get_immediate()) >= 0x800)) {
		source_register = translate_operand(batch, 1, nullptr, RiscVRegister::t2, RiscVRegister::t3, false, false, false, false);
	}

	// load the destination-register
	auto address = RiscVRegister::zero;
	auto dest_register = RiscVRegister::t0;
	if (dest_value)
		dest_register = translate_operand(batch, 0, &address, dest_register, RiscVRegister::t1, true, false, false, true);
	else if (dest.get_type() == OperandType::reg && dest.get_size() >= 4)
		dest_register = helper::map_reg(dest.get_register());

	// call the callback to apply the changes
	if(source_register != RiscVRegister::zero)
		execute_operation(batch, dest_register, source_register);
	else
		execute_operation(batch, dest_register, static_cast<intptr_t>(get_immediate()));

	// update the flags
	update_zero(batch, dest_register, dest.get_size());
	update_sign(batch, dest_register, dest.get_size(), RiscVRegister::t3);
	update_parity(batch, dest_register);

	// write the value back to the destination
	if(dest_value || dest_register == RiscVRegister::t0)
		translate_destination(batch, dest_register, address, RiscVRegister::t2, RiscVRegister::t3);
	else if(dest.get_size() < 8){
		batch += encoding::SLLI(dest_register, dest_register, 32);
		batch += encoding::SRLI(dest_register, dest_register, 32);
	}
}

void codegen::BinaryOperation::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const{
	unused_parameter(batch);
	unused_parameter(dst);
	unused_parameter(imm);
	dispatcher::Dispatcher::fault_exit("unsupported execution_operation");
}