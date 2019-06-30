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
			num_instructions += translate_ret(x86_instruction, riscv_instructions);
			return true;

		case InstructionType::PUSH:
		case InstructionType::POP:
			break;

		case InstructionType::LEA:
			//[0x123 + 0x321*8 + 0x12345678]
			riscv_instructions[num_instructions++] = encoding::MV(RiscVRegister::a1, RiscVRegister::zero);
			riscv_instructions[num_instructions++] = encoding::ADDI(RiscVRegister::a1, RiscVRegister::zero, 0x123);
			riscv_instructions[num_instructions++] = encoding::MV(RiscVRegister::a2, RiscVRegister::zero);
			riscv_instructions[num_instructions++] = encoding::ADDI(RiscVRegister::a2, RiscVRegister::zero, 0x321);

			translate_memory_operand(x86_instruction, riscv_instructions, num_instructions, 1, RiscVRegister::a0);
			break;

		case InstructionType::NOP:
			break;

		case InstructionType::MOV_IMM:
		case InstructionType::MOVABS_IMM:
		case InstructionType::MOV:
		case InstructionType::MOVSX:
		case InstructionType::MOVZX:
			num_instructions += translate_mov(x86_instruction, riscv_instructions);
			break;

		default:
			throw std::runtime_error("Unsupported instruction used.");
	}

	return false;
}


void CodeGenerator::translate_memory_operand(const fadec::Instruction& inst, size_t index, RiscVRegister reg,
											 utils::riscv_instruction_t* riscv, size_t& count) {
	if (inst.get_address_size() != 8)
		throw std::runtime_error("invalid addressing-size");

	const auto& operand = inst.get_operand(index);

	// add the scale & index
	if (inst.get_index_register() != fadec::Register::none) {
		riscv[count++] = encoding::MV(reg, register_mapping[static_cast<uint16_t>(
				inst.get_index_register())]);
		riscv[count++] = encoding::SLLI(reg, reg, inst.get_index_scale());
	} else {
		riscv[count++] = encoding::MV(reg, RiscVRegister::zero);
	}


	// add the base-register
	if (operand.get_register() != fadec::Register::none) {
		riscv[count++] = encoding::ADD(reg, reg, register_mapping[static_cast<uint16_t>(
				operand.get_register())]);
	}

	// add the displacement
	if (inst.get_displacement() > 0) {
		// less or equal than 12 bits
		if (inst.get_displacement() < 0x1000) {
			riscv[count++] = encoding::ADDI(reg, reg, static_cast<uint16_t>(
					inst.get_displacement()));
		} else {
			riscv[count++] = encoding::LUI(RiscVRegister::t6,
										   static_cast<uint32_t>(inst.get_displacement())
												   >> 12u);
			riscv[count++] = encoding::ADDI(RiscVRegister::t6, RiscVRegister::t6,
											static_cast<uint16_t>(inst.get_displacement()) &
											0x0FFFu);
			riscv[count++] = encoding::ADD(reg, reg, RiscVRegister::t6);
		}
	}
}

void CodeGenerator::write_to_register(encoding::RiscVRegister dest, encoding::RiscVRegister src, uint8_t op_size,
									  utils::riscv_instruction_t* riscv, size_t& count) {
	switch (op_size) {
		case 8:
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case 4:
			// load the and-mask into t4
			riscv[count++] = encoding::LUI(RiscVRegister::t4, 0x0fffff);
			riscv[count++] = encoding::ADDI(RiscVRegister::t4, RiscVRegister::t4, 0xfff);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(RiscVRegister::t5, RiscVRegister::t4, dest);
			riscv[count++] = encoding::XOR(dest, dest, RiscVRegister::t5);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::AND(RiscVRegister::t5, RiscVRegister::t4, src);
			riscv[count++] = encoding::OR(dest, dest, RiscVRegister::t5);
			return;
		case 2:
			// load the and-mask into t4
			riscv[count++] = encoding::LUI(RiscVRegister::t4, 0x0f);
			riscv[count++] = encoding::ADDI(RiscVRegister::t4, RiscVRegister::t4, 0xfff);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(RiscVRegister::t5, RiscVRegister::t4, dest);
			riscv[count++] = encoding::XOR(dest, dest, RiscVRegister::t5);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::AND(RiscVRegister::t5, RiscVRegister::t4, src);
			riscv[count++] = encoding::OR(dest, dest, RiscVRegister::t5);
			return;
		case 1:
			// clear the lower bits of the destination-register
			riscv[count++] = encoding::ANDI(RiscVRegister::t5, dest, 0xff);
			riscv[count++] = encoding::XOR(dest, dest, RiscVRegister::t5);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::ANDI(RiscVRegister::t5, src, 0xff);
			riscv[count++] = encoding::OR(dest, dest, RiscVRegister::t5);
			return;
		case 0:
			// load the and-mask into t4
			riscv[count++] = encoding::LUI(RiscVRegister::t4, 0x0f);
			riscv[count++] = encoding::ADDI(RiscVRegister::t4, RiscVRegister::t4, 0xf00);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(RiscVRegister::t5, RiscVRegister::t4, dest);
			riscv[count++] = encoding::XOR(dest, dest, RiscVRegister::t5);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::ANDI(RiscVRegister::t5, src, 0xff);
			riscv[count++] = encoding::SLLI(RiscVRegister::t5, RiscVRegister::t5, 8);
			riscv[count++] = encoding::OR(dest, dest, RiscVRegister::t5);
			return;
	}
}

size_t CodeGenerator::translate_mov(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv) {
	riscv[0] = LUI(RiscVRegister::a0, static_cast<uint16_t>(inst.get_immediate()));
	return 1;
}

size_t CodeGenerator::translate_ret(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv) {
	riscv[0] = JALR(RiscVRegister::zero, RiscVRegister::ra, 0);
	return 1;
}