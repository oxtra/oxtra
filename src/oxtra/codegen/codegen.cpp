#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;


CodeGenerator::CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf)
		: _args{args}, _elf{elf}, _codestore{args, elf} {}

host_addr_t CodeGenerator::translate(guest_addr_t addr) {
	/*
	 * max block size = min(next_codeblock.start, instruction offset limit)
	 *
	 * loop over instructions and decode
	 * - big switch for each instruction type
	 * - translate instruction
	 * - add risc-v instructions into code store
	 *
	 * add jump to dispatcher::host_enter
	 * return address to translated code
	 */
	auto& codeblock = _codestore.create_block();
	auto current_address = reinterpret_cast<const uint8_t*>(_elf.resolve_vaddr(addr));
	bool end_of_block;

	do {
		auto x86_instruction = Instruction{};
		if (decode(current_address, _elf.get_size(addr), DecodeMode::decode_64, addr, x86_instruction) <= 0)
			throw std::runtime_error("Failed to decode the instruction");

		current_address += x86_instruction.get_size();
		addr += x86_instruction.get_size();

		size_t num_instructions = 0;
		riscv_instruction_t riscv_instructions[max_riscv_instructions];

		end_of_block = translate_instruction(x86_instruction, riscv_instructions, num_instructions);

		// add tracing-information
		if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE) {
			char formatted_string[512];
			fadec::format(x86_instruction, formatted_string, sizeof(formatted_string));

			SPDLOG_TRACE("Fadec decoded instruction {}", formatted_string);

			for (size_t i = 0; i < num_instructions; i++)
				SPDLOG_TRACE(" - translated instruction[{}] = {}", i, decoding::parse_riscv(riscv_instructions[i]));
		}

		_codestore.add_instruction(codeblock, x86_instruction, riscv_instructions, num_instructions);
	} while (!end_of_block);

	//add dynamic tracing-information for the basic-block
	spdlog::trace("Basicblock translated: x86: [0x{0:x} - 0x{1:x}] riscv: 0x{2:x}", codeblock.x86_start,
				  codeblock.x86_end, codeblock.riscv_start);

	return codeblock.riscv_start;
}

void CodeGenerator::translate_memory_operand(const Instruction& inst, size_t index, RiscVRegister reg,
											 riscv_instruction_t* riscv, size_t& count) {
	if (inst.get_address_size() != 8)
		throw std::runtime_error("invalid addressing-size");
	const auto& operand = inst.get_operand(index);

	// add the scale & index
	if (inst.get_index_register() != fadec::Register::none) {
		riscv[count++] = encoding::MV(reg, register_mapping[static_cast<uint16_t>( inst.get_index_register())]);
		riscv[count++] = encoding::SLLI(reg, reg, inst.get_index_scale());
	} else {
		load_12bit_immediate(0, reg, riscv, count);
	}

	// add the base-register
	if (operand.get_register() != fadec::Register::none) {
		riscv[count++] = encoding::ADD(reg, reg, register_mapping[static_cast<uint16_t>( operand.get_register())]);
	}

	// add the displacement
	if (inst.get_displacement() > 0) {
		// less or equal than 12 bits
		if (inst.get_displacement() < 0x800) {
			riscv[count++] = encoding::ADDI(reg, reg, static_cast<uint16_t>(inst.get_displacement()));
		} else {
			load_unsigned_immediate(inst.get_displacement(), memory_temp_register, riscv, count);
			riscv[count++] = encoding::ADD(reg, reg, memory_temp_register);
		}
	}
}

void CodeGenerator::move_to_register(RiscVRegister dest, RiscVRegister src, uint8_t size, riscv_instruction_t* riscv,
									 size_t& count) {
	switch (size) {
		case 8:
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case 4:
			// load the and-mask into t5
			riscv[count++] = encoding::LUI(RiscVRegister::t5, 0x0fffff);
			riscv[count++] = encoding::ADDI(RiscVRegister::t5, RiscVRegister::t5, 0xfff);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(RiscVRegister::t6, RiscVRegister::t5, dest);
			riscv[count++] = encoding::XOR(dest, dest, RiscVRegister::t6);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::AND(RiscVRegister::t6, RiscVRegister::t5, src);
			riscv[count++] = encoding::OR(dest, dest, RiscVRegister::t6);
			return;
		case 2:
			// load the and-mask into t5
			riscv[count++] = encoding::LUI(RiscVRegister::t5, 0x0f);
			riscv[count++] = encoding::ADDI(RiscVRegister::t5, RiscVRegister::t5, 0xfff);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(RiscVRegister::t6, RiscVRegister::t5, dest);
			riscv[count++] = encoding::XOR(dest, dest, RiscVRegister::t6);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::AND(RiscVRegister::t6, RiscVRegister::t5, src);
			riscv[count++] = encoding::OR(dest, dest, RiscVRegister::t6);
			return;
		case 1:
			// clear the lower bits of the destination-register
			riscv[count++] = encoding::ANDI(RiscVRegister::t6, dest, 0xff);
			riscv[count++] = encoding::XOR(dest, dest, RiscVRegister::t6);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::ANDI(RiscVRegister::t6, src, 0xff);
			riscv[count++] = encoding::OR(dest, dest, RiscVRegister::t6);
			return;
		case 0:
			// load the and-mask into t5
			riscv[count++] = encoding::LUI(RiscVRegister::t5, 0x0f);
			riscv[count++] = encoding::ADDI(RiscVRegister::t5, RiscVRegister::t5, 0xf00);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(RiscVRegister::t6, RiscVRegister::t5, dest);
			riscv[count++] = encoding::XOR(dest, dest, RiscVRegister::t6);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::ANDI(RiscVRegister::t6, src, 0xff);
			riscv[count++] = encoding::SLLI(RiscVRegister::t6, RiscVRegister::t6, 8);
			riscv[count++] = encoding::OR(dest, dest, RiscVRegister::t6);
			return;
	}
}

void CodeGenerator::get_from_register(RiscVRegister dest, RiscVRegister src, uint8_t size, riscv_instruction_t* riscv,
									  size_t& count) {
	switch (size) {
		case 8:
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case 4:
			// load the and-mask into t6
			riscv[count++] = encoding::LUI(RiscVRegister::t6, 0x0fffff);
			riscv[count++] = encoding::ADDI(RiscVRegister::t6, RiscVRegister::t6, 0xfff);

			// extract the lower bits of the source-register
			riscv[count++] = encoding::AND(dest, RiscVRegister::t6, src);
			return;
		case 2:
			// load the and-mask into t6
			riscv[count++] = encoding::LUI(RiscVRegister::t6, 0x0f);
			riscv[count++] = encoding::ADDI(RiscVRegister::t6, RiscVRegister::t6, 0xfff);

			// extract the lower bits of the source-register
			riscv[count++] = encoding::AND(dest, RiscVRegister::t6, src);
			return;
		case 1:
			riscv[count++] = encoding::ANDI(dest, src, 0xff);
			return;
		case 0:
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

void CodeGenerator::load_immediate(uintptr_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
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