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
	 * add jump to dispatcher::host_enter
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

		if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE) {
			char formatted_string[512];
			fadec::format(x86_instruction, formatted_string, sizeof(formatted_string));

			SPDLOG_TRACE("Fadec decoded instruction {}", formatted_string);
		}

		size_t num_instructions = 0;
		riscv_instruction_t riscv_instructions[max_riscv_instructions];

		end_of_block = translate_instruction(x86_instruction, riscv_instructions, num_instructions);

		_codestore.add_instruction(codeblock, x86_instruction, riscv_instructions, num_instructions);
	} while (!end_of_block);

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
			//TODO: reserve register? t5?
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
			load_unsigned_immediate(x86_instruction.get_displacement(), RiscVRegister::t6,
									riscv_instructions, num_instructions);
			riscv_instructions[num_instructions++] = encoding::ADD(reg, reg, RiscVRegister::t6);
		}
	}
}

//TODO: how to handle register reservation? constexpr? document it? where to document it? Load_IMM requires t4 and t5

void CodeGenerator::load_12bit_immediate(uint16_t immediate, encoding::RiscVRegister destination,
										 utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	//TODO: is this worth the hassle?
	if (immediate == 0) {
		riscv_instructions[num_instructions++] = encoding::MV(destination, RiscVRegister::zero);
	} else {
		riscv_instructions[num_instructions++] = encoding::ADDI(destination, RiscVRegister::zero,
																static_cast<uint16_t>(immediate) & 0x0FFFu);
	}
}

void CodeGenerator::load_32bit_immediate(uint32_t immediate, encoding::RiscVRegister destination,
										 utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	// basically this method currently works by:
	// loading the upper 20 bit into destination (sets the lower 12 bit to zero and sign extends)
	// moving the lowest 12 bits into temp, and shifting out the sign
	// or the two fields
	// addi does not work if the lowest 12 bit are signed (have the 11th bit 1) because the add then carries a bit
	//TODO: there has to be a better way ... ?
	riscv_instructions[num_instructions++] = encoding::LUI(destination, static_cast<uint32_t>(immediate >> 12u));


	riscv_instructions[num_instructions++] = encoding::ADDI(RiscVRegister::t4, RiscVRegister::zero,
															static_cast<uint16_t>(immediate & 0x0FFFu));

	riscv_instructions[num_instructions++] = encoding::SLLI(RiscVRegister::t4, RiscVRegister::t4, 52);
	riscv_instructions[num_instructions++] = encoding::SRLI(RiscVRegister::t4, RiscVRegister::t4, 52);

	riscv_instructions[num_instructions++] = encoding::OR(destination, destination, RiscVRegister::t4);
}

void CodeGenerator::load_64bit_immediate(uint64_t immediate, encoding::RiscVRegister destination,
										 utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	// load upper 32bit into destination
	load_32bit_immediate(
			static_cast<uint32_t>((immediate & 0xFFFFFFFF00000000u) >> 32u),
			destination, riscv_instructions, num_instructions
	);

	riscv_instructions[num_instructions++] = encoding::SLLI(destination, destination, 32);

	// load lower 32 bit into temp
	load_32bit_immediate(
			static_cast<uint32_t>(immediate & 0xFFFFFFFFu), RiscVRegister::t5, riscv_instructions, num_instructions
	);

	// clear sign bit(s)
	riscv_instructions[num_instructions++] = encoding::SLLI(RiscVRegister::t5, RiscVRegister::t5, 32);
	riscv_instructions[num_instructions++] = encoding::SRLI(RiscVRegister::t5, RiscVRegister::t5, 32);

	riscv_instructions[num_instructions++] = encoding::OR(destination, destination, RiscVRegister::t5);
}

void CodeGenerator::load_immediate(uintptr_t immediate, encoding::RiscVRegister destination,
								   utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	if (immediate < 0x1000) {	// 12 bit can be directly encoded
		load_12bit_immediate(static_cast<uint16_t>(immediate), destination, riscv_instructions, num_instructions);
	} else if (immediate < 0x100000000) {	//32 bit have to be manually specified
		load_32bit_immediate(static_cast<uint32_t>(immediate), destination, riscv_instructions, num_instructions);
	} else { // 64 bit also have to be manually specified
		load_64bit_immediate(static_cast<uint64_t>(immediate), destination, riscv_instructions, num_instructions);
	}
}

void CodeGenerator::load_unsigned_immediate(uintptr_t immediate, encoding::RiscVRegister destination,
											utils::riscv_instruction_t* riscv_instructions, size_t& num_instructions) {
	uint8_t immediate_type = 2; // 0 means 12bit, 1 means 32 bit, >= 2 means 64 bit

	// if it is a signed immediate we have to use the bigger type to ensure that it is padded with zeros.
	// otherwise add methods may not work correctly
	if (immediate < 0x1000) {	// 12 bits can be directly encoded
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
