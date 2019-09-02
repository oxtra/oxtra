#include "instruction.h"

using namespace encoding;
using namespace fadec;
using namespace dispatcher;

codegen::Instruction::Instruction(uint8_t update, uint8_t require) {
	update_flags = update;
	require_flags = require;
}

uint8_t codegen::Instruction::query_require() {
	return require_flags;
}

uint8_t codegen::Instruction::query_update() {
	return update_flags;
}

void codegen::Instruction::set_require(uint8_t flags) {
	require_flags = flags;
}

RiscVRegister codegen::Instruction::translate_operand(CodeBatch& batch, size_t index,
													  RiscVRegister reg, RiscVRegister temp_a, RiscVRegister temp_b) {
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
												 RiscVRegister address, RiscVRegister temp_a, RiscVRegister temp_b) {
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
													 RiscVRegister temp_a, RiscVRegister temp_b) {
	if (get_address_size() < 4)
		Dispatcher::fault_exit("invalid addressing-size");
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
	if (get_displacement() > 0) {
		const auto displacement = get_displacement();
		// less or equal than 12 bits
		if (displacement < 0x800) {
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

void codegen::Instruction::move_to_register(CodeBatch& batch, RiscVRegister dest, RiscVRegister src, RegisterAccess access,
											RiscVRegister temp, bool cleared) {
	switch (access) {
		case RegisterAccess::QWORD:
			batch += encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case RegisterAccess::DWORD:
			// copy the source-register and clear the upper bits by shifting
			batch += encoding::SLLI(dest, src, 32);
			batch += encoding::SRLI(dest, dest, 32);
			return;
		case RegisterAccess::WORD:
			// check if the upper source-register has to be cleared
			if (!cleared) {
				batch += encoding::XOR(temp, src, dest);
				batch += encoding::SLLI(temp, temp, 48);
				batch += encoding::SRLI(temp, temp, 48);
				batch += encoding::XOR(dest, temp, dest);
			} else {
				// clear the lower bits of the destination-register by shifting
				batch += encoding::SRLI(dest, dest, 16);
				batch += encoding::SLLI(dest, dest, 16);
				batch += encoding::OR(dest, dest, src);
			}
			return;
		case RegisterAccess::LBYTE:
			// clear the destination
			batch += encoding::ANDI(dest, dest, -0x100);

			// extract the lower bits of the source-register and merge the registers
			if (!cleared)
				batch += encoding::ANDI(temp, src, 0xff);
			batch += encoding::OR(dest, dest, cleared ? src : temp);
			return;
		case RegisterAccess::HBYTE:
			// move the 8 bits of the destination-register down and xor them with the source
			batch += encoding::SRLI(temp, dest, 8);
			batch += encoding::XOR(temp, temp, src);

			// clear the upper 48 bits of the temp register and keep the lower 8 cleared
			batch += encoding::SLLI(temp, temp, 56);
			batch += encoding::SRLI(temp, temp, 48);

			// xor the temporary register to the destination
			batch += encoding::XOR(dest, temp, dest);
			return;
	}
}

void codegen::Instruction::load_12bit_immediate(CodeBatch& batch, uint16_t imm, RiscVRegister dest) {
	batch += encoding::ADDI(dest, RiscVRegister::zero, static_cast<uint16_t>(imm) & 0x0FFFu);
}

void codegen::Instruction::load_32bit_immediate(CodeBatch& batch, uint32_t imm, RiscVRegister dest, bool optimize) {
	auto upper_immediate = static_cast<uint32_t>(imm >> 12u);
	const auto lower_immediate = static_cast<uint16_t>(imm & 0x0FFFu);

	// adding the lower bits is sign extended, so if the lower bits are signed we have to increase the upper immediate
	if (imm & 0x800u)
		upper_immediate++;

	batch += encoding::LUI(dest, upper_immediate);

	if (!optimize || lower_immediate != 0x0000) {
		batch += encoding::ADDI(dest, dest, lower_immediate);
	}
}

void codegen::Instruction::load_64bit_immediate(CodeBatch& batch, uint64_t imm, RiscVRegister dest, bool optimize) {
	const uint32_t high_bits = (imm >> 32u) & 0xFFFFFFFF;
	const uint32_t low_bits = imm & 0xFFFFFFFF;
	constexpr size_t immediate_count = 4;
	uint32_t immediates[immediate_count] = {low_bits & 0x000000FFu, (low_bits & 0x000FFF00u) >> 8u,
											(low_bits & 0xFFF00000u) >> 20u, high_bits};

	for (size_t i = immediate_count - 2; i >= 1; i--) {
		if (immediates[i] & 0x800u)
			immediates[i + 1]++;
	}
	// load upper 32bit into destination
	load_32bit_immediate(batch, immediates[immediate_count - 1], dest, optimize);

	for (int8_t i = immediate_count - 2; i >= 0; i--) {
		// add the next 12bit (8 bit for the last one)
		batch += encoding::SLLI(dest, dest, (i == 0) ? 8 : 12);
		if (!optimize || immediates[i] != 0) {
			batch += encoding::ADDI(dest, dest, immediates[i]);
		}
	}
}


void codegen::Instruction::load_immediate(CodeBatch& batch, uintptr_t imm, encoding::RiscVRegister dest) {
	/* 00000000 00000000 00001111 11111111 22222222 22223333 33333333 44444444 */

	// < 12-bit
	if (imm < 0x800) {

	}

	// < 32-bit
	else if (imm < 0x80000000) {

	}

	// >= 32 bit
	else {

	}
}

encoding::RiscVRegister codegen::Instruction::evalute_zero(CodeBatch& batch) {
	batch += encoding::LD(RiscVRegister::t4, Instruction::context_address, FlagInfo::zero_value_offset);
	batch += encoding::SNEZ(RiscVRegister::t4, RiscVRegister::t4);
	return RiscVRegister::t4;
}

encoding::RiscVRegister codegen::Instruction::evalute_sign(CodeBatch& batch, encoding::RiscVRegister temp) {
	// load the shift amount
	batch += LBU(RiscVRegister::t4, Instruction::context_address, FlagInfo::sign_size_offset);

	// load the value
	batch += LD(temp, Instruction::context_address, FlagInfo::sign_value_offset);

	// shift the value
	batch += SRL(temp, temp, RiscVRegister::t4);
	batch += ANDI(RiscVRegister::t4, temp, 1);
	return RiscVRegister::t4;
}

encoding::RiscVRegister codegen::Instruction::evalute_parity(CodeBatch& batch) {

}

encoding::RiscVRegister codegen::Instruction::evalute_overflow(CodeBatch& batch) {

}

encoding::RiscVRegister codegen::Instruction::evalute_carry(CodeBatch& batch) {

}

void codegen::Instruction::update_zero(CodeBatch& batch, bool set) {
	// check if the instruction has to update the zero-flag
	if ((update_flags & Flags::zero) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_zero(CodeBatch& batch, encoding::RiscVRegister va, size_t size) {
	// check if the instruction has to update the zero-flag
	if ((update_flags & Flags::zero) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_sign(CodeBatch& batch, bool set) {
	// check if the instruction has to update the sign-flag
	if ((update_flags & Flags::sign) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_sign(CodeBatch& batch, encoding::RiscVRegister va, size_t size) {
	// check if the instruction has to update the sign-flag
	if ((update_flags & Flags::sign) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_parity(CodeBatch& batch, bool set) {
	// check if the instruction has to update the parity-flag
	if ((update_flags & Flags::parity) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_parity(CodeBatch& batch, encoding::RiscVRegister va, size_t size) {
	// check if the instruction has to update the parity-flag
	if ((update_flags & Flags::parity) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_overflow(CodeBatch& batch, uint16_t index, bool set) {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & Flags::overflow) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_overflow(CodeBatch& batch, uint16_t index, encoding::RiscVRegister va,
										   encoding::RiscVRegister vb) {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & Flags::overflow) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_carry(CodeBatch& batch, uint16_t index, bool set) {
	// check if the instruction has to update the carry-flag
	if ((update_flags & Flags::carry) == 0)
		return;
	/*
	 * implement the flag
	 */
}

void codegen::Instruction::update_carry(CodeBatch& batch, uint16_t index, encoding::RiscVRegister va,
										encoding::RiscVRegister vb) {
	// check if the instruction has to update the carry-flag
	if ((update_flags & Flags::carry) == 0)
		return;
	/*
	 * implement the flag
	 */
}