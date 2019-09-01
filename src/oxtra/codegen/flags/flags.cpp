#include <oxtra/codegen/jump_table.h>
#include <oxtra/dispatcher/dispatcher.h>
#include "flags.h"
#include "oxtra/codegen/encoding/encoding.h"
#include "oxtra/codegen/codegen.h"


using namespace encoding;

/*
 * All this code is untested.
 */

static uint16_t adjust_operation(uint16_t operation, uint8_t size) {
	switch (size) {
	case 0:
	case 1:
		return operation;
	case 2:
		return operation + 1;
	case 4:
		return operation + 2;
	case 8:
		return operation + 3;
	default:
		dispatcher::Dispatcher::fault_exit("Instruction size out of range");
		return 0;
	}
}

void codegen::flags::update_zero_flag(RiscVRegister reg, uint8_t reg_size,
									  utils::riscv_instruction_t* riscv, size_t& count) {
	// if the register size is 8 then we don't need to store 0 into the memory operand
	if (reg_size == 8) {
		riscv[count++] = SD(CodeGenerator::context_address, reg, FlagUpdateInfo::zf_value_offset);
		return;
	}

	// clear the zf_value because the upper bits may still be set
	riscv[count++] = SD(CodeGenerator::context_address, RiscVRegister::zero, FlagUpdateInfo::zf_value_offset);


	switch (reg_size) {
		case 1:
			riscv[count++] = SB(CodeGenerator::context_address, reg, FlagUpdateInfo::zf_value_offset);
			break;

		case 2:
			riscv[count++] = SH(CodeGenerator::context_address, reg, FlagUpdateInfo::zf_value_offset);
			break;

		case 4:
			riscv[count++] = SW(CodeGenerator::context_address, reg, FlagUpdateInfo::zf_value_offset);
			break;

		default:
			throw std::runtime_error("Invalid register size.");
	}
}

void codegen::flags::evaluate_zero_flag(bool invert, uint16_t offset, RiscVRegister temp,
										utils::riscv_instruction_t* riscv, size_t& count) {
	// load the value we base our zero flag calculation off of
	riscv[count++] = LD(temp, CodeGenerator::context_address, FlagUpdateInfo::zf_value_offset);

	// skip the instructions if the zero flag is set (zf_value == 0)
	riscv[count++] = (invert ? BNQZ : BEQZ)(temp, offset + 4);
}

void codegen::flags::update_sign_flag(RiscVRegister reg, RiscVRegister temp, uint8_t reg_size,
									  utils::riscv_instruction_t* riscv, size_t& count) {
	// store the sf_value
	riscv[count++] = SD(CodeGenerator::context_address, reg, FlagUpdateInfo::sf_value_offset);

	// store the size in bits - 1 (the evaluate function shifts by that amount to get the value of the carry flag)
	riscv[count++] = ADDI(temp, RiscVRegister::zero, reg_size * 8 - 1);
	riscv[count++] = SB(CodeGenerator::context_address, temp, FlagUpdateInfo::sf_size_offset);
}

void codegen::flags::evaluate_sign_flag(bool invert, uint16_t offset, RiscVRegister temp_a, RiscVRegister temp_b,
										utils::riscv_instruction_t* riscv, size_t& count) {
	// shift the sign bit (most significant bit) into the least significant bit

	// load the shift amount
	riscv[count++] = LBU(temp_a, CodeGenerator::context_address, FlagUpdateInfo::sf_size_offset);

	// load the value
	riscv[count++] = LD(temp_b, CodeGenerator::context_address, FlagUpdateInfo::sf_value_offset);

	// shift the value
	riscv[count++] = SRL(temp_b, temp_b, temp_a);
	riscv[count++] = ANDI(temp_b, temp_b, 1);

	// jump if the sign bit is set (sf == 1)
	riscv[count++] = (invert ? BEQZ : BNQZ)(temp_b, offset + 4);
}

void codegen::flags::update_parity_flag(RiscVRegister reg,
										utils::riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = SB(CodeGenerator::context_address, reg, FlagUpdateInfo::pf_value_offset);
}

void codegen::flags::evaluate_parity_flag(bool invert, uint16_t offset, RiscVRegister temp_a, RiscVRegister temp_b,
										  utils::riscv_instruction_t* riscv, size_t& count) {
	// load the pf_value
	riscv[count++] = LBU(temp_a, CodeGenerator::context_address, FlagUpdateInfo::pf_value_offset);

	// calculate the pf
	riscv[count++] = SRLI(temp_b, temp_a, 4);
	riscv[count++] = XOR(temp_a, temp_a, temp_b);
	riscv[count++] = SRLI(temp_b, temp_a, 2);
	riscv[count++] = XOR(temp_a, temp_a, temp_b);
	riscv[count++] = SRLI(temp_b, temp_a, 1);
	riscv[count++] = XOR(temp_a, temp_a, temp_b);

	// only look at the least significant bit
	riscv[count++] = ANDI(temp_a, temp_a, 1);

	// jump if the parity flag is set (bit is 0)
	riscv[count++] = (invert ? BNQZ : BEQZ)(temp_a, offset + 4);
}

void codegen::flags::update_carry_flag(RiscVRegister reg_a, RiscVRegister reg_b, RiscVRegister temp, uint8_t reg_size,
									   CarryOperation operation, utils::riscv_instruction_t* riscv, size_t& count) {
	const auto op = adjust_operation(static_cast<uint16_t>(operation), reg_size);

#ifdef DEBUG
	if (op >= static_cast<uint16_t>(CarryOperation::last))
		dispatcher::Dispatcher::fault_exit("Overflow operation out of range");
#endif
	// store the values
	riscv[count++] = SD(CodeGenerator::context_address, reg_a, FlagUpdateInfo::cf_values_offset);
	riscv[count++] = SD(CodeGenerator::context_address, reg_b, FlagUpdateInfo::cf_values_offset + 8);

	// store the jump table index
	riscv[count++] = ADDI(temp, RiscVRegister::zero, op * 4);
	riscv[count++] = SH(CodeGenerator::context_address, temp, FlagUpdateInfo::cf_operation_offset);
}

utils::riscv_instruction_t& codegen::flags::evaluate_carry_flag(utils::riscv_instruction_t* riscv, size_t& count) {
	// load the jump table offset
	riscv[count++] = LHU(RiscVRegister::t4, CodeGenerator::context_address, FlagUpdateInfo::cf_operation_offset);

	// jump into the jump table
	jump_table::jump_flag_evaluation(RiscVRegister::t4, RiscVRegister::ra, riscv, count);

	return riscv[count++];
}

void codegen::flags::branch_carry_flag(utils::riscv_instruction_t& instruction, bool invert, uint16_t offset) {
	// compute_carry returns the carry flag in t4
	instruction = (invert ? BEQZ : BNQZ)(RiscVRegister::t4, offset + 4);
}

void codegen::flags::update_overflow_flag(RiscVRegister reg_a, RiscVRegister reg_b, RiscVRegister temp,
										  uint8_t reg_size, OverflowOperation operation,
										  utils::riscv_instruction_t* riscv, size_t& count) {
	const auto op = adjust_operation(static_cast<uint16_t>(operation), reg_size);
#ifdef DEBUG
	if (op >= static_cast<uint16_t>(OverflowOperation::last))
		dispatcher::Dispatcher::fault_exit("Overflow operation out of range");
#endif
	// store the values
	riscv[count++] = SD(CodeGenerator::context_address, reg_a, FlagUpdateInfo::of_values_offset);
	riscv[count++] = SD(CodeGenerator::context_address, reg_b, FlagUpdateInfo::of_values_offset + 8);

	// store the jump table index
	riscv[count++] = ADDI(temp, RiscVRegister::zero,
			(op + static_cast<uint16_t>(CarryOperation::last)) * 4);
	riscv[count++] = SH(CodeGenerator::context_address, temp, FlagUpdateInfo::of_operation_offset);
}

void codegen::flags::evaluate_overflow_flag(bool invert, uint16_t offset, utils::riscv_instruction_t* riscv, size_t& count) {
	// load the jump table offset
	riscv[count++] = LHU(RiscVRegister::t4, CodeGenerator::context_address, FlagUpdateInfo::of_operation_offset);

	// jump into the jump table
	jump_table::jump_flag_evaluation(RiscVRegister::t4, RiscVRegister::ra, riscv, count);

	// compute_carry returns the carry flag in t4
	riscv[count++] = (invert ? BEQZ : BNQZ)(RiscVRegister::t4, offset + 4);
}