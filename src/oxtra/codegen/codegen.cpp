#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>
#include <oxtra/dispatcher/dispatcher.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;
using namespace dispatcher;

CodeGenerator::CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf)
		: _args{args}, _elf{elf}, _codestore{args, elf} {}

host_addr_t CodeGenerator::translate(guest_addr_t addr) {
	/* Validate the page-protection for the new basic block.
	 * Code-block-size will continue the check for the upcoming instructions.
	 * This check will also validate, that the address won't corrupt the page-array. */
	if ((_elf.get_page_flags(addr) & (elf::PAGE_EXECUTE | elf::PAGE_READ)) != (elf::PAGE_EXECUTE | elf::PAGE_READ))
		dispatcher::Dispatcher::fault_exit("virtual segmentation fault");
	if (const auto riscv_code = _codestore.find(addr))
		return riscv_code;

	// extract the next block
	auto next_block = _codestore.get_next_block(addr);

	// iterate through the instructions and query all information about the flags
	std::vector<ContextInstruction> instructions;
	while (true) {
		// check if the address is equal to the next block
		if (next_block) {
			if (next_block->x86_start == addr) {
				if (instructions.empty())
					Dispatcher::fault_exit("codestore::find(...) must have failed");
				instructions[instructions.size() - 1].update_flags |= Group::require_all;
				break;
			}
		}

		// decode the fadec-instruction
		ContextInstruction entry{};
		if (fadec::decode(reinterpret_cast<uint8_t*>(addr), _elf.get_size(addr), DecodeMode::decode_64, addr, entry) <= 0) {
			dispatcher::Dispatcher::fault_exit("Failed to decode the instruction");
		}

		// query all of the information about the instruction
		entry.update_flags = group_instruction(entry.get_type());
		if (entry.update_flags == Group::error) {
			char formatted_string[256];
			fadec::format(entry, formatted_string, sizeof(formatted_string));
			char exception_buffer[512];
			snprintf(exception_buffer, sizeof(exception_buffer), "Unsupported instruction used. %s", formatted_string);
			dispatcher::Dispatcher::fault_exit(exception_buffer);
		}

		// update the address
		addr += entry.get_size();

		// add the instruction to the array and check if the instruction would end the block
		instructions.push_back(entry);
		if ((entry.update_flags & Group::end_of_block) == Group::end_of_block) {
			next_block = nullptr;
			break;
		}
	}

	// iterate through the instructions backwards and check where the instructions have to be up-to-date
	size_t required_updates = 0;
	for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
		// extract the flags the instruction has to update
		size_t need_update = (it->update_flags & required_updates);
		required_updates ^= need_update;

		// add the requirements of this instruction to the search-requirements,
		// to indicate to previous instructions, that the flags are needed, and update its update-flags
		required_updates |= (it->update_flags & Group::require_all) << Group::require_to_update_lshift;
		it->update_flags = need_update;
	}

	// iterate through the instructions and translate them to riscv-code
	auto& codeblock = _codestore.create_block();
	riscv_instruction_t riscv[max_riscv_instructions];
	for (size_t i = 0; i < instructions.size(); i++) {
		// translate the instruction
		size_t count = 0;
		translate_instruction(instructions[i], riscv, count);

		// check if the instruction is the last instruction and the upcoming block is known
		if (next_block) {
			if (i == instructions.size() - 1) {
				// append the forcing connection to the next block
				load_unsigned_immediate(next_block->riscv_start, address_destination, riscv, count);
				riscv[count++] = encoding::JALR(RiscVRegister::zero, address_destination, 0);
			}
		}

		// print tracing-information
		if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG) {
			char formatted_string[512];
			fadec::format(instructions[i], formatted_string, sizeof(formatted_string));
			SPDLOG_DEBUG("decoded {}", formatted_string);
			for (size_t j = 0; j < count; j++) {
				SPDLOG_TRACE(" - instruction[{}] = {}", j, decoding::parse_riscv(riscv[j]));
			}
		}

		// add the instruction to the store
		_codestore.add_instruction(codeblock, instructions[i], riscv, count);
	}

	// add dynamic tracing-information for the basic-block
	spdlog::debug("Basicblock translated: x86: [0x{0:x} - 0x{1:x}] riscv: 0x{2:x}", codeblock.x86_start, codeblock.x86_end,
				  codeblock.riscv_start);

	return codeblock.riscv_start;
}

void CodeGenerator::update_basic_block(utils::host_addr_t addr, utils::host_addr_t absolute_address) {
	// compute new base-address where the new absolute address will be written to t0
	// 9 = 8 [load_64bit_immediate] + 1 [JALR]
	auto riscv = reinterpret_cast<riscv_instruction_t*>(addr - 9 * sizeof(riscv_instruction_t));
	size_t count = 0;

	// write the new instructions
	load_64bit_immediate(absolute_address, address_destination, riscv, count, false);
#ifdef DEBUG
	if (count != 8)
		dispatcher::Dispatcher::fault_exit("load_64bit_immediate did not generate 8 instructions", -1);
#endif
	riscv[count++] = encoding::JALR(RiscVRegister::zero, address_destination, 0);
}

size_t CodeGenerator::group_instruction(fadec::InstructionType type) {
	switch (type) {
		case InstructionType::ADD:
		case InstructionType::ADD_IMM:
		case InstructionType::SUB:
		case InstructionType::SUB_IMM:
		case InstructionType::NEG:
		case InstructionType::IMUL2:
		case InstructionType::SHL_CL:
		case InstructionType::SHL_IMM:
		case InstructionType::SHR_CL:
		case InstructionType::SAR_CL:
		case InstructionType::SHR_IMM:
		case InstructionType::SAR_IMM:
			return Group::update_all;

		case InstructionType::INC:
		case InstructionType::DEC:
			return Group::update_all ^ Group::update_carry;

		case InstructionType::MOV_IMM:
		case InstructionType::MOVABS_IMM:
		case InstructionType::MOV:
		case InstructionType::MOVSX:
		case InstructionType::MOVZX:
		case InstructionType::NOP:
		case InstructionType::PUSH:
		case InstructionType::POP:
			return Group::none;

		case InstructionType::PUSHF:
			return Group::require_all;

		case InstructionType::POPF:
			return Group::update_all;

		case InstructionType::JMP:
		case InstructionType::JMP_IND:
		case InstructionType::SYSCALL:
		case InstructionType::RET:
		case InstructionType::RET_IMM:
		case InstructionType::CALL:
		case InstructionType::CALL_IND:
			return Group::end_of_block;

		default:
			return Group::error;
	}
}

void CodeGenerator::translate_instruction(ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	switch (inst.get_type()) {
		case InstructionType::ADD:
		case InstructionType::ADD_IMM:
			apply_operation(inst, riscv, count, translate_add);
			break;

		case InstructionType::SUB:
		case InstructionType::SUB_IMM:
			apply_operation(inst, riscv, count, translate_sub);
			break;

		case InstructionType::NEG:
			apply_operation(inst, riscv, count, translate_neg);
			break;

		case InstructionType::INC:
			apply_operation(inst, riscv, count, translate_inc);
			break;
		case InstructionType::DEC:
			apply_operation(inst, riscv, count, translate_dec);
			break;

		case InstructionType::IMUL2:
			apply_operation(inst, riscv, count, translate_imul);
			break;

		case InstructionType::SHL_CL:
		case InstructionType::SHL_IMM:
			apply_operation(inst, riscv, count, translate_shl);
			break;

		case InstructionType::SHR_CL:
		case InstructionType::SHR_IMM:
			apply_operation(inst, riscv, count, translate_shr);
			break;

		case InstructionType::SAR_CL:
		case InstructionType::SAR_IMM:
			apply_operation(inst, riscv, count, translate_sar);
			break;

		case InstructionType::MOV_IMM:
		case InstructionType::MOVABS_IMM:
		case InstructionType::MOV:
			translate_mov(inst, riscv, count);
			break;

		case InstructionType::MOVSX:
		case InstructionType::MOVZX:
			apply_operation(inst, riscv, count, translate_mov_ext);
			break;

		case InstructionType::NOP:
			break;

		case InstructionType::PUSH:
			translate_push(inst, riscv, count);
			break;

		case InstructionType::PUSHF:
			translate_pushf(inst, riscv, count);
			break;

		case InstructionType::POP:
			translate_pop(inst, riscv, count);
			break;

		case InstructionType::POPF:
			translate_popf(inst, riscv, count);
			break;

		case InstructionType::SYSCALL:
			translate_syscall(inst, riscv, count);
			break;

		case InstructionType::CALL:
		case InstructionType::CALL_IND:
			translate_call(inst, riscv, count);
			break;

		case InstructionType::RET:
		case InstructionType::RET_IMM:
			translate_ret(inst, riscv, count);
			break;

		case InstructionType::JMP:
		case InstructionType::JMP_IND:
			translate_jmp(inst, riscv, count);
			break;

		default:
			break;
	}
}