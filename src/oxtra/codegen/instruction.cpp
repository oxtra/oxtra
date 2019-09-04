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

RiscVRegister codegen::Instruction::translate_operand(CodeBatch& batch, size_t index,
													  RiscVRegister reg, RiscVRegister temp_a, RiscVRegister temp_b) const {
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

void codegen::Instruction::translate_destination(CodeBatch& batch, RiscVRegister reg,
												 RiscVRegister address, RiscVRegister temp_a, RiscVRegister temp_b) const {
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

RiscVRegister codegen::Instruction::translate_memory(CodeBatch& batch, size_t index,
													 RiscVRegister temp_a, RiscVRegister temp_b) const {
	if (get_address_size() < 4)
		dispatcher::Dispatcher::fault_exit("invalid addressing-size");
	const auto& operand = get_operand(index);

	// check if its only a base-register
	if (get_index_register() == fadec::Register::none && get_displacement() == 0 && operand.get_size() == 8)
		return map_reg(operand.get_register());

	// add the scale & index
	RiscVRegister temp_reg = RiscVRegister::zero;
	if (get_index_register() != fadec::Register::none) {
		batch += encoding::SLLI(temp_a, map_reg(get_index_register()), get_index_scale());
		temp_reg = temp_a;
	}

	// add the base-register
	if (operand.get_register() != fadec::Register::none) {
		batch += encoding::ADD(temp_a, temp_reg, map_reg(operand.get_register()));
		temp_reg = temp_a;
	}

	// add the displacement
	if (const auto displacement = get_displacement()) {
		// less or equal than 12 bits
		if (displacement >= -0x800 && displacement < 0x800) {
			batch += encoding::ADDI(temp_a, temp_reg, static_cast<uint16_t>(displacement));
			if (temp_reg == RiscVRegister::zero)
				temp_reg = temp_a;
		} else {
			if (temp_reg == RiscVRegister::zero) {
				load_immediate(batch, displacement, temp_a);
				temp_reg = temp_a;
			} else {
				load_immediate(batch, displacement, temp_b);
				batch += encoding::ADD(temp_a, temp_reg, temp_b);
			}
		}
	}

	// check if the addressing-mode is a 32-bit mode
	if (get_address_size() == 4) {
		batch += encoding::SLLI(temp_reg, temp_reg, 32);
		batch += encoding::SRLI(temp_reg, temp_reg, 32);
	}
	return temp_reg;
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

void codegen::Instruction::update_sign(CodeBatch& batch, encoding::RiscVRegister va,
									   uint8_t size, encoding::RiscVRegister temp) const {
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

	batch += encoding::ADDI(temp, RiscVRegister::zero,static_cast<uint16_t>(
			set ? jump_table::Entry::overflow_set : jump_table::Entry::overflow_clear) * 4);
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