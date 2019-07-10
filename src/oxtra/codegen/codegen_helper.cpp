#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;

void CodeGenerator::translate_memory_operand(const Instruction& inst, size_t index, RiscVRegister reg,
											 riscv_instruction_t* riscv, size_t& count) {
	if (inst.get_address_size() < 4)
		throw std::runtime_error("invalid addressing-size");
	const auto& operand = inst.get_operand(index);

	// add the scale & index
	if (inst.get_index_register() != fadec::Register::none) {
		if (inst.get_address_size() == 8)
			get_from_register(reg, register_mapping[static_cast<uint16_t>( inst.get_index_register())],
							  RegisterAccess::QWORD, riscv, count);
		else
			get_from_register(reg, register_mapping[static_cast<uint16_t>( inst.get_index_register())],
							  RegisterAccess::DWORD, riscv, count);
		riscv[count++] = encoding::SLLI(reg, reg, inst.get_index_scale());
	} else
		load_unsigned_immediate(0, reg, riscv, count);

	// add the base-register
	if (operand.get_register() != fadec::Register::none) {
		if (inst.get_address_size() == 8)
			riscv[count++] = encoding::ADD(reg, reg, register_mapping[static_cast<uint16_t>( operand.get_register())]);
		else {
			get_from_register(memory_temp_register, register_mapping[static_cast<uint16_t>(operand.get_register())],
							  RegisterAccess::DWORD, riscv, count);
			riscv[count++] = encoding::ADD(reg, reg, memory_temp_register);
		}
	}

	// add the displacement
	if (inst.get_displacement() > 0 || _elf.get_address_delta() > 0) {
		uintptr_t displacement = _elf.get_address_delta() + inst.get_displacement();
		// less or equal than 12 bits
		if (displacement < 0x800) {
			riscv[count++] = encoding::ADDI(reg, reg, static_cast<uint16_t>(displacement));
		} else {
			load_unsigned_immediate(displacement, memory_temp_register, riscv, count);
			riscv[count++] = encoding::ADD(reg, reg, memory_temp_register);
		}
	}
}

void CodeGenerator::move_to_register(RiscVRegister dest, RiscVRegister src, RegisterAccess access,
									 riscv_instruction_t* riscv, size_t& count) {
	switch (access) {
		case RegisterAccess::QWORD:
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case RegisterAccess::DWORD:
			// clear the lower bits of the destination-register by shifting
			riscv[count++] = encoding::SRLI(dest, dest, 32);
			riscv[count++] = encoding::SLLI(dest, dest, 32);

			// copy the source-register and clear the upper bits by shifting
			riscv[count++] = encoding::SLLI(read_temp_register, src, 32);
			riscv[count++] = encoding::SRLI(read_temp_register, read_temp_register, 32);

			// combine the registers
			riscv[count++] = encoding::OR(dest, dest, read_temp_register);
			return;
		case RegisterAccess::WORD:
			// clear the lower bits of the destination-register by shifting
			riscv[count++] = encoding::SRLI(dest, dest, 16);
			riscv[count++] = encoding::SLLI(dest, dest, 16);

			// copy the source-register and clear the upper bits by shifting
			riscv[count++] = encoding::SLLI(read_temp_register, src, 48);
			riscv[count++] = encoding::SRLI(read_temp_register, read_temp_register, 48);

			// combine the registers
			riscv[count++] = encoding::OR(dest, dest, read_temp_register);
			return;
		case RegisterAccess::LBYTE:
			// clear the lower bits of the destination-register
			riscv[count++] = encoding::ANDI(read_temp_register, dest, 0xff);
			riscv[count++] = encoding::XOR(dest, dest, read_temp_register);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::ANDI(read_temp_register, src, 0xff);
			riscv[count++] = encoding::OR(dest, dest, read_temp_register);
			return;
		case RegisterAccess::HBYTE:
			// load the and-mask
			load_unsigned_immediate(0xff00, read_temp_register, riscv, count);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(read_temp_register, read_temp_register, dest);
			riscv[count++] = encoding::XOR(dest, dest, read_temp_register);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::ANDI(read_temp_register, src, 0xff);
			riscv[count++] = encoding::SLLI(read_temp_register, read_temp_register, 8);
			riscv[count++] = encoding::OR(dest, dest, read_temp_register);
			return;
	}
}

void CodeGenerator::get_from_register(RiscVRegister dest, RiscVRegister src, RegisterAccess access,
									  riscv_instruction_t* riscv, size_t& count) {
	switch (access) {
		case RegisterAccess::QWORD:
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case RegisterAccess::DWORD:
			// load copy the register and shift-clear it
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			riscv[count++] = encoding::SLLI(dest, dest, 32);
			riscv[count++] = encoding::SRLI(dest, dest, 32);
			return;
		case RegisterAccess::WORD:
			// load copy the register and shift-clear it
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			riscv[count++] = encoding::SLLI(dest, dest, 48);
			riscv[count++] = encoding::SRLI(dest, dest, 48);
			return;
		case RegisterAccess::LBYTE:
			riscv[count++] = encoding::ANDI(dest, src, 0xff);
			return;
		case RegisterAccess::HBYTE:
			riscv[count++] = encoding::SRLI(dest, src, 8);
			riscv[count++] = encoding::ANDI(dest, dest, 0xff);
			return;
	}
}

void CodeGenerator::load_12bit_immediate(uint16_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = encoding::ADDI(dest, RiscVRegister::zero, static_cast<uint16_t>(imm) & 0x0FFFu);
}

void CodeGenerator::load_32bit_immediate(uint32_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	auto upper_immediate = static_cast<uint32_t>(imm >> 12u);
	const auto lower_immediate = static_cast<uint16_t>(imm & 0x0FFFu);

	// adding the lower bits is sign extended, so if the lower bits are signed we have to increase the upper immediate
	if (imm & 0x800u)
		upper_immediate++;

	riscv[count++] = encoding::LUI(dest, upper_immediate);

	if (lower_immediate != 0x0000) {
		riscv[count++] = encoding::ADDI(dest, dest, lower_immediate);
	}
}

void CodeGenerator::load_64bit_immediate(uint64_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
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
	load_32bit_immediate(immediates[immediate_count - 1], dest, riscv, count);

	for (int8_t i = immediate_count - 2; i >= 0; i--) {
		// add the next 12bit (8 bit for the last one)
		riscv[count++] = encoding::SLLI(dest, dest, (i == 0) ? 8 : 12);
		if (immediates[i] != 0) {
			riscv[count++] = encoding::ADDI(dest, dest, immediates[i]);
		}
	}
}

void
CodeGenerator::load_signed_immediate(uintptr_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
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
		load_32bit_immediate(static_cast<uint32_t>(imm), dest, riscv, count);
	} else { // 64 bit also have to be manually specified
		load_64bit_immediate(static_cast<uint64_t>(imm), dest, riscv, count);
	}
}

void
CodeGenerator::load_unsigned_immediate(uintptr_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
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
		load_32bit_immediate(static_cast<uint32_t>(imm), dest, riscv, count);
	} else {
		load_64bit_immediate(static_cast<uint64_t>(imm), dest, riscv, count);
	}
}