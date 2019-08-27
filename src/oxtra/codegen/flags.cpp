#include "flags.h"
#include "encoding/encoding.h"
#include "codegen.h"


using namespace encoding;

/*
 * All this code is untested.
 */

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

void codegen::flags::evaluate_zero_flag(RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {
	// load the value we base our zero flag calculation off of
	riscv[count++] = LD(RiscVRegister::t0, CodeGenerator::context_address, FlagUpdateInfo::zf_value_offset);

	// reg = (t0 == 0) ? 1 : 0
	riscv[count++] = SEQZ(reg, RiscVRegister::t0);
}

void codegen::flags::update_sign_flag(RiscVRegister reg, uint8_t reg_size,
									  utils::riscv_instruction_t* riscv, size_t& count) {
	// store the sf_value
	riscv[count++] = SD(CodeGenerator::context_address, reg, FlagUpdateInfo::sf_value_offset);

	// store the size
	riscv[count++] = ADDI(RiscVRegister::t0, RiscVRegister::zero, reg_size);
	riscv[count++] = SB(CodeGenerator::context_address, RiscVRegister::t0, FlagUpdateInfo::sf_size_offset);
}

void codegen::flags::evaluate_sign_flag(RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {
	// shift the sign bit (most significant bit) into the least significant bit

	// load the reg size
	riscv[count++] = LBU(RiscVRegister::t0, CodeGenerator::context_address, FlagUpdateInfo::sf_size_offset);

	// we wanna shit for one less bit
	riscv[count++] = ADDI(RiscVRegister::t0, RiscVRegister::t0, -1);

	// load the value
	riscv[count++] = LD(reg, CodeGenerator::context_address, FlagUpdateInfo::sf_value_offset);

	// shift the value
	riscv[count++] = SLL(reg, reg, RiscVRegister::t0);
}

void codegen::flags::update_parity_flag(RiscVRegister reg,
										utils::riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = SB(CodeGenerator::context_address, reg, FlagUpdateInfo::pf_value_offset);
}

void codegen::flags::evaluate_parity_flag(RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {
	// load the pf_value
	riscv[count++] = LBU(RiscVRegister::t0, CodeGenerator::context_address, FlagUpdateInfo::pf_value_offset);

	// calculate the pf
	riscv[count++] = SRLI(RiscVRegister::t1, RiscVRegister::t0, 4);
	riscv[count++] = XOR(RiscVRegister::t0, RiscVRegister::t0, RiscVRegister::t1);
	riscv[count++] = SRLI(RiscVRegister::t1, RiscVRegister::t0, 2);
	riscv[count++] = XOR(RiscVRegister::t0, RiscVRegister::t0, RiscVRegister::t1);
	riscv[count++] = SRLI(RiscVRegister::t1, RiscVRegister::t0, 1);
	riscv[count++] = XOR(RiscVRegister::t0, RiscVRegister::t0, RiscVRegister::t1);

	// only look at the least significant bit
	riscv[count++] = ANDI(RiscVRegister::t0, RiscVRegister::t0, 1);
	riscv[count++] = SEQZ(reg, RiscVRegister::t0);
}

void codegen::flags::update_carry_flag(RiscVRegister reg_a, RiscVRegister reg_b, uint8_t reg_size,
									   FlagOperation operation, utils::riscv_instruction_t* riscv, size_t& count) {
	// store the values
	riscv[count++] = SD(CodeGenerator::context_address, reg_a, FlagUpdateInfo::cf_values_offset);
	riscv[count++] = SD(CodeGenerator::context_address, reg_b, FlagUpdateInfo::cf_values_offset + 8);

	// store the store the register sizes involved
	riscv[count++] = ADDI(RiscVRegister::t0, RiscVRegister::zero, reg_size);
	riscv[count++] = SB(RiscVRegister::t0, CodeGenerator::context_address, FlagUpdateInfo::cf_size_offset);

	// store the operation
	riscv[count++] = ADDI(RiscVRegister::t0, RiscVRegister::zero, static_cast<uint16_t>(operation));
	riscv[count++] = SB(CodeGenerator::context_address, RiscVRegister::t0, FlagUpdateInfo::cf_operation_offset);
}

void codegen::flags::evaluate_carry_flag(RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {
	// do this in software
}

void codegen::flags::update_overflow_flag(RiscVRegister reg_a, RiscVRegister reg_b, uint8_t reg_size,
										  FlagOperation operation, utils::riscv_instruction_t* riscv, size_t& count) {
	// store the values
	riscv[count++] = SD(CodeGenerator::context_address, reg_a, FlagUpdateInfo::of_values_offset);
	riscv[count++] = SD(CodeGenerator::context_address, reg_b, FlagUpdateInfo::of_values_offset + 8);

	// store the store the register sizes involved
	riscv[count++] = ADDI(RiscVRegister::t0, RiscVRegister::zero, reg_size);
	riscv[count++] = SB(CodeGenerator::context_address, RiscVRegister::t0, FlagUpdateInfo::of_size_offset);

	// store the operation
	riscv[count++] = ADDI(RiscVRegister::t0, RiscVRegister::zero, static_cast<uint16_t>(operation));
	riscv[count++] = SB(CodeGenerator::context_address, RiscVRegister::t0, FlagUpdateInfo::of_operation_offset);
}

void codegen::flags::evaluate_overflow_flag(RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {
	// do this in software
}