#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>
#include <oxtra/dispatcher/dispatcher.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;
using namespace dispatcher;

void CodeGenerator::apply_operation(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count,
									OperationCallback callback) {
	// extract the source-operand
	RiscVRegister source_register = RiscVRegister::t0;
	if (inst.get_operand(1).get_type() == OperandType::reg &&
		inst.get_operand(1).get_register_type() != RegisterType::gph)
		source_register = map_reg(inst.get_operand(1).get_register());
	else
		translate_operand(inst, 1, source_register, RiscVRegister::t2, RiscVRegister::t3, riscv, count);

	// extract the register for the destination-value
	RiscVRegister dest_register = RiscVRegister::t1;
	RiscVRegister address = RiscVRegister::zero;
	if (inst.get_operand(0).get_type() == OperandType::reg && inst.get_operand(0).get_size() >= 4)
		dest_register = map_reg(inst.get_operand(0).get_register());
	else
		address = translate_operand(inst, 0, dest_register, RiscVRegister::t2, RiscVRegister::t3, riscv, count);

	// call the callback to apply the changes
	callback(inst, dest_register, source_register, riscv, count);

	// write the value back to the destination
	translate_destination(inst, dest_register, address, RiscVRegister::t2, RiscVRegister::t3, riscv, count);
}

encoding::RiscVRegister
CodeGenerator::translate_operand(const fadec::Instruction& inst, size_t index, RiscVRegister reg,
								 RiscVRegister temp_a, RiscVRegister temp_b, utils::riscv_instruction_t* riscv, size_t& count) {
	// extract the operand
	auto& operand = inst.get_operand(index);

	// load the source-operand into the temporary-register
	if (operand.get_type() == OperandType::reg) {
		/* read the value from the register (read the whole register
		 * (unless HBYTE is required), and just cut the rest when writing the register */
		if (operand.get_register_type() == RegisterType::gph) {
			riscv[count++] = encoding::SRLI(reg, map_reg_high(operand.get_register()), 8);
		} else
			riscv[count++] = encoding::ADD(reg, map_reg(operand.get_register()), RiscVRegister::zero);
	} else if (operand.get_type() == OperandType::imm)
		load_unsigned_immediate(inst.get_immediate(), reg, riscv, count);
	else {
		// read the value from memory
		translate_memory(inst, index, temp_a, temp_b, riscv, count);
		switch (operand.get_size()) {
			case 8:
				riscv[count++] = encoding::LD(reg, temp_a, 0);
				break;
			case 4:
				riscv[count++] = encoding::LW(reg, temp_a, 0);
				break;
			case 2:
				riscv[count++] = encoding::LH(reg, temp_a, 0);
				break;
			case 1:
				riscv[count++] = encoding::LB(reg, temp_a, 0);
				break;
		}
		return temp_a;
	}
	return encoding::RiscVRegister::zero;
}

void CodeGenerator::translate_destination(const fadec::Instruction& inst, RiscVRegister reg, RiscVRegister address,
										  RiscVRegister temp_a, RiscVRegister temp_b, utils::riscv_instruction_t* riscv,
										  size_t& count) {
	auto& operand = inst.get_operand(0);

	// check if the destination is a register
	if (operand.get_type() == OperandType::reg) {
		RiscVRegister temp_reg = map_reg(operand.get_register());
		switch (operand.get_size()) {
			case 8:
				if (temp_reg != reg)
					move_to_register(temp_reg, reg, RegisterAccess::QWORD, temp_b, riscv, count);
				break;
			case 4:
				move_to_register(temp_reg, reg, RegisterAccess::DWORD, temp_b, riscv, count);
				break;
			case 2:
				move_to_register(temp_reg, reg, RegisterAccess::WORD, temp_b, riscv, count);
				break;
			case 1:
				if (operand.get_register_type() == RegisterType::gph) {
					move_to_register(map_reg_high(operand.get_register()), reg, RegisterAccess::HBYTE, temp_b, riscv, count);
				} else {
					move_to_register(temp_reg, reg, RegisterAccess::LBYTE, temp_b, riscv, count);
				}
				break;
		}
		return;
	}

	// translate the memory-address and write the value to it
	if (address == encoding::RiscVRegister::zero) {
		translate_memory(inst, 0, temp_a, temp_b, riscv, count);
		address = temp_a;
	}
	switch (operand.get_size()) {
		case 8:
			riscv[count++] = encoding::SD(address, reg, 0);
			break;
		case 4:
			riscv[count++] = encoding::SW(address, reg, 0);
			break;
		case 2:
			riscv[count++] = encoding::SH(address, reg, 0);
			break;
		case 1:
			riscv[count++] = encoding::SB(address, reg, 0);
			break;
	}
}

void CodeGenerator::translate_memory(const Instruction& inst, size_t index, RiscVRegister reg, RiscVRegister temp,
									 riscv_instruction_t* riscv, size_t& count) {
	if (inst.get_address_size() < 4)
		Dispatcher::fault_exit("invalid addressing-size");
	const auto& operand = inst.get_operand(index);

	// add the scale & index
	RiscVRegister temp_reg = RiscVRegister::zero;
	if (inst.get_index_register() != fadec::Register::none) {
		riscv[count++] = encoding::SLLI(reg, map_reg(inst.get_index_register()), inst.get_index_scale());
		temp_reg = reg;
	}

	// add the base-register
	if (operand.get_register() != fadec::Register::none) {
		riscv[count++] = encoding::ADD(reg, temp_reg, map_reg(operand.get_register()));
		temp_reg = reg;
	}

	// add the displacement
	if (inst.get_displacement() > 0) {
		const auto displacement = inst.get_displacement();
		// less or equal than 12 bits
		if (displacement < 0x800) {
			riscv[count++] = encoding::ADDI(reg, temp_reg, static_cast<uint16_t>(displacement));
		} else {
			load_unsigned_immediate(displacement, temp, riscv, count);
			riscv[count++] = encoding::ADD(reg, temp_reg, temp);
		}
	}
}

void CodeGenerator::move_to_register(RiscVRegister dest, RiscVRegister src, RegisterAccess access, RiscVRegister temp,
									 riscv_instruction_t* riscv, size_t& count, bool cleared) {
	switch (access) {
		case RegisterAccess::QWORD:
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case RegisterAccess::DWORD:
			// copy the source-register and clear the upper bits by shifting
			riscv[count++] = encoding::SLLI(dest, src, 32);
			riscv[count++] = encoding::SRLI(dest, dest, 32);
			return;
		case RegisterAccess::WORD:
			// clear the lower bits of the destination-register by shifting
			riscv[count++] = encoding::SRLI(dest, dest, 16);
			riscv[count++] = encoding::SLLI(dest, dest, 16);

			// copy the source-register and clear the upper bits by shifting
			if (!cleared) {
				riscv[count++] = encoding::SLLI(temp, src, 48);
				riscv[count++] = encoding::SRLI(temp, temp, 48);
			}

			// combine the registers
			riscv[count++] = encoding::OR(dest, dest, cleared ? src : temp);
			return;
		case RegisterAccess::LBYTE:
			// clear the lower bits of the destination-register
			riscv[count++] = encoding::ANDI(temp, dest, 0xff);
			riscv[count++] = encoding::XOR(dest, dest, temp);

			// extract the lower bits of the source-register and merge the registers
			if (!cleared) {
				riscv[count++] = encoding::ANDI(temp, src, 0xff);
			}
			riscv[count++] = encoding::OR(dest, dest, cleared ? src : temp);
			return;
		case RegisterAccess::HBYTE:
			// load the and-mask
			load_unsigned_immediate(0xff00, temp, riscv, count);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(temp, temp, dest);
			riscv[count++] = encoding::XOR(dest, dest, temp);

			// extract the lower bits of the source-register and merge the registers
			if (!cleared) {
				riscv[count++] = encoding::ANDI(temp, src, 0xff);
			}
			riscv[count++] = encoding::SLLI(temp, cleared ? src : temp, 8);
			riscv[count++] = encoding::OR(dest, dest, temp);
			return;
	}
}

void CodeGenerator::load_12bit_immediate(uint16_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = encoding::ADDI(dest, RiscVRegister::zero, static_cast<uint16_t>(imm) & 0x0FFFu);
}

void CodeGenerator::load_32bit_immediate(uint32_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count,
										 bool optimize) {
	auto upper_immediate = static_cast<uint32_t>(imm >> 12u);
	const auto lower_immediate = static_cast<uint16_t>(imm & 0x0FFFu);

	// adding the lower bits is sign extended, so if the lower bits are signed we have to increase the upper immediate
	if (imm & 0x800u)
		upper_immediate++;

	riscv[count++] = encoding::LUI(dest, upper_immediate);

	if (!optimize || lower_immediate != 0x0000) {
		riscv[count++] = encoding::ADDI(dest, dest, lower_immediate);
	}
}

void CodeGenerator::load_64bit_immediate(uint64_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count,
										 bool optimize) {
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
	load_32bit_immediate(immediates[immediate_count - 1], dest, riscv, count, optimize);

	for (int8_t i = immediate_count - 2; i >= 0; i--) {
		// add the next 12bit (8 bit for the last one)
		riscv[count++] = encoding::SLLI(dest, dest, (i == 0) ? 8 : 12);
		if (!optimize || immediates[i] != 0) {
			riscv[count++] = encoding::ADDI(dest, dest, immediates[i]);
		}
	}
}

void CodeGenerator::load_signed_immediate(uintptr_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	uintptr_t short_value = (imm & 0xFFFu);
	if (short_value & 0x800u) {
		short_value |= 0xFFFFFFFFFFFFF000;
	}

	uintptr_t word_value = imm & 0xFFFFFFFFu;
	if (word_value & 0x80000000) {
		word_value |= 0xFFFFFFFF00000000;
	}

	if (imm == short_value) {    // 12 bit can be directly encoded
		load_12bit_immediate(static_cast<uint16_t>(imm), dest, riscv, count);
	} else if (imm == word_value) {    //32 bit have to be manually specified
		load_32bit_immediate(static_cast<uint32_t>(imm), dest, riscv, count, true);
	} else { // 64 bit also have to be manually specified
		load_64bit_immediate(static_cast<uint64_t>(imm), dest, riscv, count, true);
	}
}

void CodeGenerator::load_unsigned_immediate(uintptr_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	uint8_t immediate_type = 2; // 0 means 12bit, 1 means 32 bit, >= 2 means 64 bit

	// if it is a signed immediate we have to use the bigger type to ensure that it is padded with zeros.
	// otherwise add methods may not work correctly
	if (imm < 0x1000) {    // 12 bits can be directly encoded
		immediate_type = imm & 0x0800u ? 1 : 0; // but only if it is a positive 12 bit (sign bit not set)
	} else if (imm < 0x100000000) {
		immediate_type = imm & 0x80000000 ? 2 : 1; // same with 32 bit
	}

	if (immediate_type == 0) {
		load_12bit_immediate(static_cast<uint16_t>(imm), dest, riscv, count);
	} else if (immediate_type == 1) {
		load_32bit_immediate(static_cast<uint32_t>(imm), dest, riscv, count, true);
	} else {
		load_64bit_immediate(static_cast<uint64_t>(imm), dest, riscv, count, true);
	}
}