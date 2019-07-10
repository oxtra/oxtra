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

	// iterate through the instructions and query all information about the flags
	std::vector<InstructionEntry> instructions;
	auto address = reinterpret_cast<const uint8_t*>(_elf.resolve_vaddr(addr));
	while (true) {
		// decode the fadec-instruction
		InstructionEntry entry{};
		if (fadec::decode(address, _elf.get_size(addr), DecodeMode::decode_64, addr, entry.instruction) <= 0)
			throw std::runtime_error("Failed to decode the instruction");

		// query all of the information about the instruction
		entry.update_flags = translate_instruction(entry, nullptr, nullptr);
		if (entry.update_flags == Group::error)
			throw std::runtime_error("Unsupported instruction used.");

		// update the addresses
		addr += entry.instruction.get_size();
		address += entry.instruction.get_size();

		// add the instruction to the array and check if the instruction would end the block
		instructions.push_back(entry);
		if ((entry.update_flags & Group::end_of_block) == Group::end_of_block)
			break;
	}

	// iterate through the instructions backwards and check where the instructions have to be up-to-date
	size_t required_updates = 0;
	for (size_t i = instructions.size(); i > 0; i--) {
		// extract the flags the instruction has to update
		size_t need_update = (instructions[i - 1].update_flags & required_updates);
		required_updates ^= need_update;

		// add the requirements of this instruction to the search-requirements,
		// to indicate to previous instructions, that the flags are needed, and update its update-flags
		required_updates |= (instructions[i - 1].update_flags & Group::require_all) << Group::require_to_update_lshift;
		instructions[i - 1].update_flags = need_update;
	}

	// iterate through the instructions and translate them to riscv-code
	auto& codeblock = _codestore.create_block();
	riscv_instruction_t riscv[max_riscv_instructions];
	for (auto& entry : instructions) {
		// translate the instruction
		size_t count = 0;
		translate_instruction(entry, riscv, &count);

		// print tracing-information
		if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE) {
			char formatted_string[512];
			fadec::format(entry.instruction, formatted_string, sizeof(formatted_string));
			SPDLOG_TRACE("decoded {}", formatted_string);
			for (size_t i = 0; i < count; i++)
				SPDLOG_TRACE(" - instruction[{}] = {}", i, decoding::parse_riscv(riscv[i]));
		}

		// add the instruction to the store
		_codestore.add_instruction(codeblock, entry.instruction, riscv, count);
	}

	//add dynamic tracing-information for the basic-block
	spdlog::trace("Basicblock translated: x86: [0x{0:x} - 0x{1:x}] riscv: 0x{2:x}", codeblock.x86_start, codeblock.x86_end,
				  codeblock.riscv_start);
	return codeblock.riscv_start;
}

size_t CodeGenerator::translate_instruction(InstructionEntry& inst, utils::riscv_instruction_t* riscv, size_t* count) {
	switch (inst.instruction.get_type()) {
		case InstructionType::CALL:
			return Group::end_of_block;
		case InstructionType::JMP:
			return Group::end_of_block;
		case InstructionType::LEA:
			if (riscv != nullptr) {
				load_unsigned_immediate(0xFFFFFFFF, RiscVRegister::a1, riscv, count[0]);
				load_unsigned_immediate(0x321, RiscVRegister::a2, riscv, count[0]);
				memory_operand(inst.instruction, 1, RiscVRegister::a0, riscv, count[0]);
			}
			return Group::none;

		case InstructionType::MOV_IMM:
		case InstructionType::MOVABS_IMM:
		case InstructionType::MOV:
		case InstructionType::MOVSX:
		case InstructionType::MOVZX:
			if (riscv != nullptr) {
				translate_mov(inst.instruction, riscv, count[0]);
			}
			return Group::none;

		case InstructionType::NOP:
			return Group::none;

		case InstructionType::PUSH:
		case InstructionType::POP:
			return Group::none;

		case InstructionType::RET:
		case InstructionType::RET_IMM:
			if (riscv != nullptr) {
				translate_ret(inst.instruction, riscv, count[0]);
			}
			return Group::end_of_block;
		default:
			return Group::error;
	}
}

void CodeGenerator::translate_mov(const Instruction& inst, riscv_instruction_t* riscv, size_t& count) {
	const auto& dest_operand = inst.get_operand(0);
	const auto& source_operand = inst.get_operand(1);
	fadec::OperandType tp[2] = {dest_operand.get_type(), source_operand.get_type()};

	// extract the register, where the source-value should be stored in
	RiscVRegister source_register = temp0_register;
	if (tp[0] == OperandType::reg && dest_operand.get_size() == 8)
		source_register = register_mapping[static_cast<uint16_t>(dest_operand.get_register())];

	// load the source-operand into the temporary-register
	if (tp[1] == OperandType::reg) {
		RiscVRegister src_register = register_mapping[static_cast<uint16_t>(source_operand.get_register())];

		/* read the value from the register (read the whole register
		 * (unless HBYTE is required), and just cut the rest when writing the register */
		if (source_operand.get_register_type() == RegisterType::gph) {
			if (source_operand.get_register() == Register::ah)
				get_from_register(source_register, register_mapping[static_cast<uint16_t>(Register::rax)],
								  RegisterAccess::HBYTE, riscv, count);
			else if (source_operand.get_register() == Register::bh)
				get_from_register(source_register, register_mapping[static_cast<uint16_t>(Register::rbx)],
								  RegisterAccess::HBYTE, riscv, count);
			else if (source_operand.get_register() == Register::ch)
				get_from_register(source_register, register_mapping[static_cast<uint16_t>(Register::rcx)],
								  RegisterAccess::HBYTE, riscv, count);
			else
				get_from_register(source_register, register_mapping[static_cast<uint16_t>(Register::rdx)],
								  RegisterAccess::HBYTE, riscv, count);
		} else
			get_from_register(source_register, src_register, RegisterAccess::QWORD, riscv, count);

		/* if this in struction is a movsx/movzx instruction,
		 * the input operands will vary in size. Thus they will have to be sign-extended/zero-extended.
		 * Otherwise the optimization will fail (load full 8-byte register, and store the interesting parts).
		 * [It will fail, because the interesting, stored parts, are larger than they should be]
		 * With a simple hack of shifting all the way up, and down again, we can fill the space with the
		 * highest bit. */
		if (inst.get_type() == InstructionType::MOVSX || inst.get_type() == InstructionType::MOVZX) {
			riscv[count++] = encoding::SLLI(source_register, source_register, 64 - 8 * source_operand.get_size());
			if (inst.get_type() == InstructionType::MOVSX)
				riscv[count++] = encoding::SRAI(source_register, source_register, 64 - 8 * source_operand.get_size());
			else
				riscv[count++] = encoding::SRLI(source_register, source_register, 64 - 8 * source_operand.get_size());
		}
	} else if (tp[1] == OperandType::imm)
		load_unsigned_immediate(inst.get_immediate(), source_register, riscv, count);
	else {
		// read the value from memory
		memory_operand(inst, 1, source_register, riscv, count);
		switch (source_operand.get_size()) {
			case 8:
				riscv[count++] = encoding::LD(source_register, source_register, 0);
				break;
			case 4:
				if (inst.get_type() == InstructionType::MOVSX)
					riscv[count++] = encoding::LW(source_register, source_register, 0);
				else
					riscv[count++] = encoding::LWU(source_register, source_register, 0);
				break;
			case 2:
				if (inst.get_type() == InstructionType::MOVSX)
					riscv[count++] = encoding::LH(source_register, source_register, 0);
				else
					riscv[count++] = encoding::LHU(source_register, source_register, 0);
				break;
			case 1:
				if (inst.get_type() == InstructionType::MOVSX)
					riscv[count++] = encoding::LB(source_register, source_register, 0);
				else
					riscv[count++] = encoding::LBU(source_register, source_register, 0);
				break;
		}
	}

	// write the source-register to the destination-register
	if (tp[0] == OperandType::reg) {
		RiscVRegister dst_register = register_mapping[static_cast<uint16_t>(dest_operand.get_register())];
		switch (dest_operand.get_size()) {
			case 8:
				break;
			case 4:
				move_to_register(dst_register, source_register, RegisterAccess::DWORD, riscv, count);
				break;
			case 2:
				move_to_register(dst_register, source_register, RegisterAccess::WORD, riscv, count);
				break;
			case 1:
				if (dest_operand.get_register_type() == RegisterType::gph) {
					if (dest_operand.get_register() == Register::ah)
						move_to_register(register_mapping[static_cast<uint16_t>(Register::rax)], source_register,
										 RegisterAccess::HBYTE, riscv, count);
					else if (dest_operand.get_register() == Register::bh)
						move_to_register(register_mapping[static_cast<uint16_t>(Register::rbx)], source_register,
										 RegisterAccess::HBYTE, riscv, count);
					else if (dest_operand.get_register() == Register::ch)
						move_to_register(register_mapping[static_cast<uint16_t>(Register::rcx)], source_register,
										 RegisterAccess::HBYTE, riscv, count);
					else
						move_to_register(register_mapping[static_cast<uint16_t>(Register::rdx)], source_register,
										 RegisterAccess::HBYTE, riscv, count);
				} else
					move_to_register(dst_register, source_register, RegisterAccess::LBYTE, riscv, count);
				break;
		}
		return;
	}

	// write the source-operand to memory
	memory_operand(inst, 0, temp1_register, riscv, count);
	switch (dest_operand.get_size()) {
		case 8:
			riscv[count++] = encoding::SD(temp1_register, source_register, 0);
			break;
		case 4:
			riscv[count++] = encoding::SW(temp1_register, source_register, 0);
			break;
		case 2:
			riscv[count++] = encoding::SH(temp1_register, source_register, 0);
			break;
		case 1:
			riscv[count++] = encoding::SB(temp1_register, source_register, 0);
			break;
	}
}

void CodeGenerator::translate_ret(const Instruction& inst, riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = JALR(RiscVRegister::zero, RiscVRegister::ra, 0);
}