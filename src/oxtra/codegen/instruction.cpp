#include <oxtra/dispatcher/dispatcher.h>
#include "instruction.h"
#include "oxtra/codegen/helper.h"

using namespace encoding;
using namespace fadec;
using namespace codegen::helper;

codegen::Instruction::Instruction(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool eob)
		: fadec::Instruction{inst} {
	update_flags = update;
	require_flags = eob ? Flags::all : require;
	end_of_block = eob;
}

uint8_t codegen::Instruction::get_require() const {
	return require_flags;
}

uint8_t codegen::Instruction::get_update() const {
	return update_flags;
}

bool codegen::Instruction::get_eob() const {
	return end_of_block;
}

void codegen::Instruction::set_update(uint8_t flags) {
	update_flags = flags;
}

void codegen::Instruction::set_eob() {
	end_of_block = true;
	require_flags = Flags::all;
}

std::string codegen::Instruction::string() const {
	char buffer[256];
	fadec::format(*this, buffer, sizeof(buffer));

	return buffer;
}

RiscVRegister codegen::Instruction::translate_operand(CodeBatch& batch, size_t index, RiscVRegister reg, RiscVRegister temp_a,
													  RiscVRegister temp_b) const {
	// extract the operand
	auto& operand = get_operand(index);

	// load the source-operand into the temporary-register
	if (operand.get_type() == OperandType::reg) {
		/* read the value from the register (read the whole register
		 * (unless HBYTE is required), and just cut the rest when writing the register */
		if (operand.get_register_type() == RegisterType::gph) {
			batch += encoding::SRLI(reg, map_reg_high(operand.get_register()), 8);
		} else
			batch += encoding::ADD(reg, map_reg(operand.get_register()), RiscVRegister::zero);
	} else if (operand.get_type() == OperandType::imm)
		load_immediate(batch, get_immediate(), reg);
	else if (operand.get_type() == OperandType::mem) {
		// read the value from memory
		temp_a = translate_memory(batch, index, temp_a, temp_b);
		switch (operand.get_size()) {
			case 8:
				batch += encoding::LD(reg, temp_a, 0);
				break;
			case 4:
				batch += encoding::LW(reg, temp_a, 0);
				break;
			case 2:
				batch += encoding::LH(reg, temp_a, 0);
				break;
			case 1:
				batch += encoding::LB(reg, temp_a, 0);
				break;
		}
		return temp_a;
	}
	return encoding::RiscVRegister::zero;
}

RiscVRegister codegen::Instruction::load_operand(codegen::CodeBatch& batch, size_t index,
										 RiscVRegister reg, RiscVRegister temp_a, RiscVRegister temp_b,
										 bool sign_extend) const {
	const auto& operand = get_operand(index);
	uint8_t shamt = 64 - operand.get_size() * 8;

	if (operand.get_type() == OperandType::reg) {
		const auto gph = operand.get_register_type() == RegisterType::gph;
		const auto destination_register = (gph ? map_reg_high : map_reg)(operand.get_register());

		return load_register(batch, destination_register, reg, helper::operand_to_register_access(operand), sign_extend);
	} else if (operand.get_type() == OperandType::mem) {
		temp_a = translate_memory(batch, index, temp_a, temp_b);
		switch (operand.get_size()) {
			case 8:
				batch += encoding::LD(reg, temp_a, 0);
				break;
			case 4:
				batch += encoding::LW(reg, temp_a, 0);
				break;
			case 2:
				batch += encoding::LH(reg, temp_a, 0);
				break;
			case 1:
				batch += encoding::LB(reg, temp_a, 0);
				break;
		}
		// memory instructions sign extend per default, so we undo it (if required)
		if (!sign_extend && operand.get_size() != 8) {
			batch += SLLI(reg, reg, shamt);
			batch += SRLI(reg, reg, shamt);
		}
	} else if (operand.get_type() == OperandType::imm) {
		load_immediate(batch, get_immediate(), reg);
		// Immediates are interpreted sign_extended by both riscv and fadec, so we do nothing in here
	}

	return reg;
}

void
codegen::Instruction::translate_destination(CodeBatch& batch, RiscVRegister reg, RiscVRegister address, RiscVRegister temp_a,
											RiscVRegister temp_b) const {
	auto& operand = get_operand(0);

	// check if the destination is a register
	if (operand.get_type() == OperandType::reg) {
		const auto temp_reg = map_reg(operand.get_register());
		switch (operand.get_size()) {
			case 8:
				if (temp_reg != reg)
					move_to_register(batch, temp_reg, reg, RegisterAccess::QWORD, temp_b);
				break;
			case 4:
				move_to_register(batch, temp_reg, reg, RegisterAccess::DWORD, temp_b);
				break;
			case 2:
				move_to_register(batch, temp_reg, reg, RegisterAccess::WORD, temp_b);
				break;
			case 1:
				if (operand.get_register_type() == RegisterType::gph) {
					move_to_register(batch, map_reg_high(operand.get_register()), reg, RegisterAccess::HBYTE, temp_b);
				} else {
					move_to_register(batch, temp_reg, reg, RegisterAccess::LBYTE, temp_b);
				}
				break;
		}
		return;
	}

	// check if the destination is a memory-operation
	if (operand.get_type() != OperandType::mem)
		return;

	// translate the memory-address and write the value to it
	if (address == encoding::RiscVRegister::zero)
		address = translate_memory(batch, 0, temp_a, temp_b);
	switch (operand.get_size()) {
		case 8:
			batch += encoding::SD(address, reg, 0);
			break;
		case 4:
			batch += encoding::SW(address, reg, 0);
			break;
		case 2:
			batch += encoding::SH(address, reg, 0);
			break;
		case 1:
			batch += encoding::SB(address, reg, 0);
			break;
	}
}

RiscVRegister
codegen::Instruction::translate_memory(CodeBatch& batch, size_t index, RiscVRegister temp_a, RiscVRegister temp_b) const {
	if (get_address_size() < 4)
		dispatcher::Dispatcher::fault_exit("invalid addressing-size");
	const auto& operand = get_operand(index);

	// analyze the input (0 = doesn't exist, 1 = optimizable, 2 = unoptimizable)
	uint8_t disp_exists = get_displacement() == 0 ? 0 : (get_displacement() >= -0x800 && get_displacement() < 0x800 ? 1 : 2);
	uint8_t index_exists = get_index_register() == fadec::Register::none ? 0 : (get_index_scale() == 1 ? 1 : 2);
	uint8_t base_exists = operand.get_register() == fadec::Register::none ? 0 : (get_address_size() == 8 && disp_exists == 0 &&
																				 index_exists == 0 ? 1 : 2);
	uint64_t disp_mask = get_address_size() == 8 ? 0xffffffffffffffffull : 0x00000000ffffffffull;

	// verify the operands
	if (disp_exists == 0 && index_exists == 0 && base_exists == 0)
		dispatcher::Dispatcher::fault_exit("invalid addressing-mode");

	// build the output
	RiscVRegister result = temp_a;
	if (base_exists) {
		// [base + ???]
		if (disp_exists == 1) {
			// [base + sDisp + ???]
			batch += encoding::ADDI(result, map_reg(operand.get_register()), get_displacement());

			if (index_exists == 1)
				// [base + sDisp + index]
				batch += encoding::ADD(result, result, map_reg(get_index_register()));
			else if (index_exists == 2) {
				// [base + sDisp + index * n]
				batch += encoding::SLLI(temp_b, map_reg(get_index_register()), get_index_scale());
				batch += encoding::ADD(result, result, temp_b);
			}
		} else if (index_exists == 1) {
			// [base + index*1 + ???]
			batch += encoding::ADD(result, map_reg(operand.get_register()), map_reg(get_index_register()));

			if (disp_exists) {
				// [base + index*1 + lDisp]
				helper::load_immediate(batch, get_displacement() & disp_mask, temp_b);
				batch += encoding::ADD(result, result, temp_b);
			}
		} else if (base_exists == 1)
			// [base]
			result = map_reg(operand.get_register());
		else {
			// nothing can be optimized
			batch += encoding::ADD(result, RiscVRegister::zero, map_reg(operand.get_register()));
			if (disp_exists) {
				helper::load_immediate(batch, get_displacement() & disp_mask, temp_b);
				batch += encoding::ADD(result, result, temp_b);
			}
			if (index_exists) {
				batch += encoding::SLLI(temp_b, map_reg(get_index_register()), get_index_scale());
				batch += encoding::ADD(result, result, temp_b);
			}
		}
	} else if (index_exists && disp_exists)
		if (index_exists == 1 && disp_exists == 1)
			// [index*1 + sDisp]
			batch += encoding::ADDI(result, map_reg(get_index_register()), get_displacement());
		else {
			// [index*n + lDisp]
			batch += encoding::SLLI(result, map_reg(get_index_register()), get_index_scale());
			helper::load_immediate(batch, get_displacement() & disp_mask, temp_b);
			batch += encoding::ADD(result, result, temp_b);
		}
	else if (index_exists) {
		// [index*n]
		if (get_address_size() == 8 && index_exists == 1)
			result = map_reg(get_index_register());
		else if (index_exists == 1)
			batch += encoding::ADD(result, RiscVRegister::zero, map_reg(get_index_register()));
		else
			batch += encoding::SLLI(result, map_reg(get_index_register()), get_index_scale());
	} else
		// [disp]
		load_immediate(batch, get_displacement() & disp_mask, result);

	// check if the addressing-mode is a 32-bit mode
	if (get_address_size() == 4 && (index_exists || base_exists)) {
		batch += encoding::SLLI(result, result, 32);
		batch += encoding::SRLI(result, result, 32);
	}
	return result;
}

void codegen::Instruction::evaluate_zero(CodeBatch& batch) const {
	batch += encoding::LD(RiscVRegister::t4, context_address, FlagInfo::zero_value_offset);
	batch += encoding::SEQZ(RiscVRegister::t4, RiscVRegister::t4);
}

void codegen::Instruction::evaluate_sign(CodeBatch& batch, encoding::RiscVRegister temp) const {
	// load the shift amount
	batch += encoding::LBU(RiscVRegister::t4, context_address, FlagInfo::sign_size_offset);

	// load the value
	batch += encoding::LD(temp, context_address, FlagInfo::sign_value_offset);

	// shift the value
	batch += encoding::SRL(temp, temp, RiscVRegister::t4);
	batch += encoding::ANDI(RiscVRegister::t4, temp, 1);
}

void codegen::Instruction::evaluate_parity(CodeBatch& batch, encoding::RiscVRegister temp) const {
	// load the pf_value
	batch += encoding::LBU(temp, helper::context_address, FlagInfo::parity_value_offset);

	// calculate the pf
	batch += encoding::SRLI(RiscVRegister::t4, temp, 4);
	batch += encoding::XOR(temp, temp, RiscVRegister::t4);
	batch += encoding::SRLI(RiscVRegister::t4, temp, 2);
	batch += encoding::XOR(temp, temp, RiscVRegister::t4);
	batch += encoding::SRLI(RiscVRegister::t4, temp, 1);
	batch += encoding::XOR(temp, temp, RiscVRegister::t4);

	// only look at the least significant bit
	batch += encoding::ANDI(temp, temp, 1);

	// set if the parity flag is set (bit is 0)
	batch += encoding::SEQZ(RiscVRegister::t4, temp);
}

void codegen::Instruction::evaluate_overflow(CodeBatch& batch) const {
	// load the jump table offset
	batch += LHU(RiscVRegister::t4, helper::context_address, FlagInfo::overflow_operation_offset);

	// jump into the jump table
	jump_table::jump_table_offset(batch, RiscVRegister::t4);
}

void codegen::Instruction::evaluate_carry(CodeBatch& batch) const {
	// load the jump table offset
	batch += LHU(RiscVRegister::t4, helper::context_address, FlagInfo::carry_operation_offset);

	// jump into the jump table
	jump_table::jump_table_offset(batch, RiscVRegister::t4);
}

void codegen::Instruction::update_zero(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the zero-flag
	if ((update_flags & Flags::zero) == 0)
		return;

	if (set) {
		batch += encoding::SD(helper::context_address, RiscVRegister::zero, FlagInfo::zero_value_offset);
	} else {
		batch += encoding::ADDI(temp, encoding::RiscVRegister::zero, 1);
		batch += encoding::SD(helper::context_address, temp, FlagInfo::zero_value_offset);
	}
}

void codegen::Instruction::update_zero(CodeBatch& batch, encoding::RiscVRegister va, uint8_t size) const {
	// check if the instruction has to update the zero-flag
	if ((update_flags & Flags::zero) == 0)
		return;

	// if the register size is 8 then we don't need to store 0 into the memory operand
	if (size == 8) {
		batch += encoding::SD(helper::context_address, va, FlagInfo::zero_value_offset);
		return;
	}

	// clear the zf_value because the upper bits may still be set
	batch += SD(helper::context_address, RiscVRegister::zero, FlagInfo::zero_value_offset);

	switch (size) {
		case 1:
			batch += SB(helper::context_address, va, FlagInfo::zero_value_offset);
			break;

		case 2:
			batch += SH(helper::context_address, va, FlagInfo::zero_value_offset);
			break;

		case 4:
			batch += SW(helper::context_address, va, FlagInfo::zero_value_offset);
			break;

		default:
			throw std::runtime_error("Invalid register size.");
	}

}

void codegen::Instruction::update_sign(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the sign-flag
	if ((update_flags & Flags::sign) == 0)
		return;

	if (set) {
		batch += encoding::ADDI(temp, encoding::RiscVRegister::zero, -1);
		batch += encoding::SD(helper::context_address, temp, FlagInfo::sign_value_offset);
	} else {
		batch += encoding::SD(helper::context_address, RiscVRegister::zero, FlagInfo::sign_value_offset);
	}
}

void codegen::Instruction::update_sign(CodeBatch& batch, encoding::RiscVRegister va, uint8_t size,
									   encoding::RiscVRegister temp) const {
	// check if the instruction has to update the sign-flag
	if ((update_flags & Flags::sign) == 0)
		return;

	// store the sf_value
	batch += encoding::SD(helper::context_address, va, FlagInfo::sign_value_offset);

	// store the size in bits - 1 (the evaluate function shifts by that amount to get the value of the carry flag)
	batch += encoding::ADDI(temp, RiscVRegister::zero, size * 8 - 1);
	batch += encoding::SB(helper::context_address, temp, FlagInfo::sign_size_offset);

}

void codegen::Instruction::update_parity(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the parity-flag
	if ((update_flags & Flags::parity) == 0)
		return;

	if (set) {
		batch += encoding::SB(helper::context_address, RiscVRegister::zero, FlagInfo::parity_value_offset);
	} else {
		batch += encoding::ADDI(temp, RiscVRegister::zero, 1);
		batch += encoding::SB(helper::context_address, temp, FlagInfo::parity_value_offset);
	}
}

void codegen::Instruction::update_parity(CodeBatch& batch, encoding::RiscVRegister va) const {
	// check if the instruction has to update the parity-flag
	if ((update_flags & Flags::parity) == 0)
		return;

	batch += encoding::SB(helper::context_address, va, FlagInfo::parity_value_offset);
}

void codegen::Instruction::update_overflow(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & Flags::overflow) == 0)
		return;

	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(
															   set ? jump_table::Entry::overflow_set
																   : jump_table::Entry::overflow_clear) * 4);
	batch += encoding::SH(helper::context_address, temp, FlagInfo::overflow_operation_offset);
}

void codegen::Instruction::update_overflow(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va,
										   encoding::RiscVRegister vb, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & Flags::overflow) == 0)
		return;

	// store the values
	batch += encoding::SD(helper::context_address, va, FlagInfo::overflow_values_offset);
	batch += encoding::SD(helper::context_address, vb, FlagInfo::overflow_values_offset + 8);

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(entry) * 4);
	batch += encoding::SH(helper::context_address, temp, FlagInfo::overflow_operation_offset);
}

void codegen::Instruction::update_carry(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the carry-flag
	if ((update_flags & Flags::carry) == 0)
		return;

	batch += encoding::ADDI(temp, RiscVRegister::zero,
							static_cast<uint16_t>(set ? jump_table::Entry::carry_set : jump_table::Entry::carry_clear) * 4);
	batch += encoding::SH(helper::context_address, temp, FlagInfo::carry_operation_offset);
}

void codegen::Instruction::update_carry(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va,
										encoding::RiscVRegister vb, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the carry-flag
	if ((update_flags & Flags::carry) == 0)
		return;

	// store the values
	batch += encoding::SD(helper::context_address, va, FlagInfo::carry_values_offset);
	batch += encoding::SD(helper::context_address, vb, FlagInfo::carry_values_offset + 8);

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(entry) * 4);
	batch += encoding::SH(helper::context_address, temp, FlagInfo::carry_operation_offset);
}