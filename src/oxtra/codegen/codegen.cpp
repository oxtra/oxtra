#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>
#include <math.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;


CodeGenerator::CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf)
		: _args{args}, _elf{elf}, _codestore{args, elf} {}

host_addr_t CodeGenerator::translate(guest_addr_t addr) {
	//const auto cached_code = _codestore.find(addr);
	//if (cached_code)
	//	return cached_code;

	//const auto next_codeblock = _codestore.get_next_block(addr);
	//if (next_codeblock == nullptr) {
	//	next_codeblock =
	//}

	/*
	 * max block size = min(next_codeblock.start, instruction offset limit)
	 *
	 * loop over instructions and decode
	 * - big switch for each instruction type
	 * - translate instruction
	 * - add risc-v instructions into code store
	 *
	 * add jump to dispatcher::host_call
	 * return address to translated code
	 */

	/*
	 * the program that's being decoded looks like this
	 * 0000000000401000 <_start>:
	 *  401000:	55                   	push   %rbp
	 *  401001:	48 89 e5             	mov    %rsp,%rbp
	 *  401004:	b8 2a 00 00 00       	mov    $0x2a,%eax
	 *  401009:	5d                   	pop    %rbp
	 *  40100a:	c3                   	retq
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

bool CodeGenerator::translate_instruction(const fadec::Instruction& x86_instruction,
										  utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	switch (x86_instruction.get_type()) {
		// at the moment we just insert a return for every instruction that modifies control flow.
		case InstructionType::JMP:
		case InstructionType::CALL:
		case InstructionType::RET:
		case InstructionType::RET_IMM:
			num_instructions = translate_ret(x86_instruction, riscv_instructions);
			return true;

		case InstructionType::PUSH:
		case InstructionType::POP:
			break;

		case InstructionType::LEA:
			//[0xFFFFFFFF + 0x321*8 + 0x12345678] = 0x1_1234_6F7F
			load_unsigned_immediate(0xFFFFFFFF, RiscVRegister::a1, riscv_instructions, num_instructions);
			load_unsigned_immediate(0x321, RiscVRegister::a2, riscv_instructions, num_instructions);
			translate_memory_operand(x86_instruction, 1, RiscVRegister::a0, riscv_instructions, num_instructions);
			break;

		case InstructionType::NOP:
		case InstructionType::MOV:
			break;

		case InstructionType::MOV_IMM:
		case InstructionType::MOVABS_IMM:
			num_instructions = translate_mov(x86_instruction, riscv_instructions);
			break;

		default:
			throw std::runtime_error("Unsupported instruction used.");
	}

	return false;
}

size_t
CodeGenerator::translate_mov(const fadec::Instruction& x86_instruction, utils::riscv_instruction_t* riscv_instruction) {
	riscv_instruction[0] = LUI(RiscVRegister::a0, static_cast<uint16_t>(x86_instruction.get_immediate()));
	return 1;
}

size_t
CodeGenerator::translate_ret(const fadec::Instruction& x86_instruction, utils::riscv_instruction_t* riscv_instruction) {
	riscv_instruction[0] = JALR(RiscVRegister::zero, RiscVRegister::ra, 0);
	return 1;
}

void CodeGenerator::translate_memory_operand(const fadec::Instruction& x86_instruction, size_t index, RiscVRegister reg,
											 utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	if (x86_instruction.get_address_size() != 8)
		throw std::runtime_error("invalid addressing-size");

	const auto& operand = x86_instruction.get_operand(index);

	// add the scale & index
	if (x86_instruction.get_index_register() != fadec::Register::none) {
		riscv_instructions[num_instructions++] = encoding::MV(reg, register_mapping[static_cast<uint16_t>(
				x86_instruction.get_index_register())]);
		riscv_instructions[num_instructions++] = encoding::SLLI(reg, reg, x86_instruction.get_index_scale());
	} else {
		load_12bit_immediate(0, reg, riscv_instructions, num_instructions);
	}

	// add the base-register
	if (operand.get_register() != fadec::Register::none) {
		riscv_instructions[num_instructions++] = encoding::ADD(reg, reg, register_mapping[static_cast<uint16_t>(
				operand.get_register())]);
	}

	// add the displacement
	if (x86_instruction.get_displacement() > 0) {
		// less or equal than 12 bits
		if (x86_instruction.get_displacement() < 0x800) {
			riscv_instructions[num_instructions++] = encoding::ADDI(reg, reg, static_cast<uint16_t>(
					x86_instruction.get_displacement()));
		} else {
			load_unsigned_immediate(x86_instruction.get_displacement(), memory_temp_register,
									riscv_instructions, num_instructions);
			riscv_instructions[num_instructions++] = encoding::ADD(reg, reg, memory_temp_register);
		}
	}
}

void CodeGenerator::load_12bit_immediate(uint16_t immediate, encoding::RiscVRegister destination,
										 utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	riscv_instructions[num_instructions++] = encoding::ADDI(destination, RiscVRegister::zero,
															static_cast<uint16_t>(immediate) & 0x0FFFu);
}

void CodeGenerator::load_32bit_immediate(uint32_t immediate, encoding::RiscVRegister destination,
										 utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	auto upper_immediate = static_cast<uint32_t>(immediate >> 12u);
	const auto lower_immediate = static_cast<uint16_t>(immediate & 0x0FFFu);

	// adding the lower bits is sign extended, so if the lower bits are signed we have to increase the upper immediate
	if (immediate & 0x800u) {
		upper_immediate++;
	}

	riscv_instructions[num_instructions++] = encoding::LUI(destination, upper_immediate);

	if (lower_immediate != 0x0000) {
		riscv_instructions[num_instructions++] = encoding::ADDI(destination, destination, lower_immediate);
	}
}

void CodeGenerator::load_64bit_immediate(uint64_t immediate, encoding::RiscVRegister destination,
										 utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	const uint32_t high_bits = (immediate >> 32u) & 0xFFFFFFFF;
	const uint32_t low_bits = immediate & 0xFFFFFFFF;
	constexpr size_t immediate_count = 4;
	uint32_t immediates[immediate_count] = {low_bits & 0x000000FFu, (low_bits & 0x000FFF00u) >> 8u,
											(low_bits & 0xFFF00000u) >> 20u, high_bits};

	for (size_t i = immediate_count - 2; i >= 1; i--) {
		if (immediates[i] & 0x800u) {
			immediates[i + 1]++;
		}
	}
	// load upper 32bit into destination
	load_32bit_immediate(immediates[immediate_count - 1], destination, riscv_instructions, num_instructions);

	for (int8_t i = immediate_count - 2; i >= 0; i--) {
		// add the next 12bit (8 bit for the last one)
		riscv_instructions[num_instructions++] = encoding::SLLI(destination, destination, (i == 0) ? 8 : 12);
		if (immediates[i] != 0) {
			riscv_instructions[num_instructions++] = encoding::ADDI(destination, destination, immediates[i]);
		}
	}
}

void CodeGenerator::load_immediate(uintptr_t immediate, encoding::RiscVRegister destination,
								   utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	uintptr_t short_value = (immediate & 0xFFFu);
	if (short_value & 0x800u) {
		short_value |= 0xFFFFFFFFFFFFF000;
	}

	uintptr_t word_value = immediate & 0xFFFFFFFFu;
	if (word_value & 0x80000000) {
		word_value |= 0xFFFFFFFF00000000;
	}

	if (immediate == short_value) {    // 12 bit can be directly encoded
		load_12bit_immediate(static_cast<uint16_t>(immediate), destination, riscv_instructions, num_instructions);
	} else if (immediate == word_value) {    //32 bit have to be manually specified
		load_32bit_immediate(static_cast<uint32_t>(immediate), destination, riscv_instructions, num_instructions);
	} else { // 64 bit also have to be manually specified
		load_64bit_immediate(static_cast<uint64_t>(immediate), destination, riscv_instructions, num_instructions);
	}
}

void CodeGenerator::load_unsigned_immediate(uintptr_t immediate, encoding::RiscVRegister destination,
											utils::riscv_instruction_t* riscv_instructions,
											size_t& num_instructions) {
	uint8_t immediate_type = 2; // 0 means 12bit, 1 means 32 bit, >= 2 means 64 bit

	// if it is a signed immediate we have to use the bigger type to ensure that it is padded with zeros.
	// otherwise add methods may not work correctly
	if (immediate < 0x1000) {    // 12 bits can be directly encoded
		immediate_type = immediate & 0x0800u ? 1 : 0; // but only if it is a positive 12 bit (sign bit not set)
	} else if (immediate < 0x100000000) {
		immediate_type = immediate & 0x80000000 ? 2 : 1; // same with 32 bit
	}

	if (immediate_type == 0) {
		load_12bit_immediate(static_cast<uint16_t>(immediate), destination, riscv_instructions, num_instructions);
	} else if (immediate_type == 1) {
		load_32bit_immediate(static_cast<uint32_t>(immediate), destination, riscv_instructions, num_instructions);
	} else {
		load_64bit_immediate(static_cast<uint64_t>(immediate), destination, riscv_instructions, num_instructions);
	}
}
