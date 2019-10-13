#include "oxtra/codegen/codegen.h"
#include "oxtra/dispatcher/dispatcher.h"
#include "oxtra/debugger/debugger.h"
#include "oxtra/logger/logger.h"
#include "helper.h"

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;
using namespace dispatcher;

CodeGenerator::CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf)
		: _elf{elf}, _codestore{args, elf}, _call_table{CallEntry{}} {
	// instantiate the code-batch
	if (args.get_debugging())
		_batch = std::make_unique<debugger::DebuggerBatch>();
	else
		_batch = std::make_unique<CodeBatchImpl>();
}

host_addr_t CodeGenerator::translate(guest_addr_t addr) {
	/* Validate the page-protection for the new basic block.
	 * Code-block-size will continue the check for the upcoming instructions.
	 * This check will also validate, that the address won't corrupt the page-array. */
	if ((_elf.get_page_flags(addr) & (elf::PAGE_EXECUTE | elf::PAGE_READ)) != (elf::PAGE_EXECUTE | elf::PAGE_READ))
		Dispatcher::fault_exit("virtual segmentation fault");

	if (const auto riscv_code = _codestore.find(addr))
		return riscv_code;

	// extract the next block
	auto next_block = _codestore.get_next_block(addr);

	// iterate through the instructions and query all information about the flags
	std::vector<std::unique_ptr<Instruction>> instructions;
	while (true) {
		// check if the address is equal to the next block
		if (next_block) {
			if (next_block->x86_start == addr) {
				if (instructions.empty()) {
					instructions.~vector();
					Dispatcher::fault_exit("codestore::find(...) must have failed");
				}
				break;
			}
		}

		// decode the fadec-instruction
		auto&& inst = decode_instruction(addr, instructions);

		// check if the instruction would end the block
		if (inst.get_eob()) {
			next_block = nullptr;
			break;
		}
	}

	// set the flags for the first instruction
	size_t required_updates = flags::all;
	if (const uintptr_t rec_addr = instructions.back()->recursive_require(); rec_addr != 0) {
		required_updates &= ~instructions.back()->get_update();
		required_updates = recursive_flag_requirements(required_updates, rec_addr, 4);
		required_updates |= instructions.back()->get_require();
	}

	// iterate through the instructions backwards and check where the instructions have to be up-to-date
	for (auto it = instructions.rbegin() + 1; it != instructions.rend(); ++it) {
		// get the instruction
		auto&& inst = *it;

		// extract the flags the instruction has to update
		size_t need_update = inst->get_update() & required_updates;
		required_updates ^= need_update;

		// add the requirements of this instruction to the search-requirements,
		// to indicate to previous instructions, that the flags are needed, and update its update-flags
		required_updates |= inst->get_require();

		// check if the instruction has recursive requirements
		if (const uintptr_t rec_addr = inst->recursive_require(); rec_addr != 0) {
			uintptr_t require = flags::all & ~inst->get_update();
			required_updates |= recursive_flag_requirements(require, rec_addr, 4);
		}
		inst->set_update(need_update);
	}

	// initialize the basic block for the debugger
	_batch->reset();
	debugger::Debugger::begin_block(*_batch);

	// iterate through the instructions and translate them to riscv-code
	auto&& codeblock = _codestore.create_block();
	for (size_t i = 0; i < instructions.size(); i++) {
		auto&& inst = instructions[i];
		_batch->reset();

		// add the instruction to the debugger
		debugger::Debugger::insert(*_batch, inst.get());

		// translate the instruction
		inst->generate(*_batch);

		// check if the instruction is the last instruction and the upcoming block is known
		if (next_block) {
			if (i == instructions.size() - 1) {
				// append the forcing connection to the next block
				helper::load_immediate(*_batch, next_block->riscv_start, helper::address_destination);
				*_batch += encoding::JALR(RiscVRegister::zero, helper::address_destination, 0);
			}
		}
		_batch->end();

		// print some debug-information
		if (logger::get_level(logger::Level::x86)) {
			logger::log(logger::Level::x86, "  {}\n", inst->string());
		}

		if (logger::get_level(logger::Level::riscv)) {
			_batch->print();
		}

		// add the instruction to the store
		_codestore.add_instruction(codeblock, inst->get_address(), inst->get_size(), _batch->get(), _batch->size());
	}

	// finalize the basic block for the debugger
	debugger::Debugger::end_block(*_batch, &codeblock);

	// add dynamic tracing-information for the basic-block
	logger::log(logger::Level::translated, "basicblock: [{0:#x} - {1:#x}] -> [{2:#x}]\n", codeblock.x86_start,
				codeblock.x86_end,
				codeblock.riscv_start);

	return codeblock.riscv_start;
}

void CodeGenerator::update_basic_block(utils::host_addr_t addr, utils::host_addr_t absolute_address) {
	// compute new base-address where the new absolute address will be written to t0
	// 9 = 8 [load_64bit_immediate] + 1 [JALR] ( + 8 * debug_step_entry)
	const size_t batch_size = (debugger::Debugger::step_riscv() ? 17 : 9);
	const auto start = addr - batch_size * sizeof(riscv_instruction_t);
	CodeMemory code{reinterpret_cast<riscv_instruction_t*>(start), batch_size};

	// write the new instructions
	auto diff = absolute_address - start;

	if (std::abs(static_cast<ptrdiff_t>(diff)) <= 0x7fff'ffff) {
		const auto lo = diff & 0xfffu;
		auto hi = diff >> 12u;
		if (lo & 0x800u) {
			hi++;
		}

		code += encoding::AUIPC(helper::address_destination, hi);
		code += encoding::JALR(RiscVRegister::zero, helper::address_destination, lo);
	} else {
		helper::load_immediate(code, absolute_address, helper::address_destination);
		code += encoding::JALR(RiscVRegister::zero, helper::address_destination, 0);
	}
}

CallEntry* CodeGenerator::get_call_table() {
	return _call_table.data();
}

codegen::Instruction& CodeGenerator::decode_instruction(utils::guest_addr_t& addr, inst_vec_t& inst_vec) const {
	// decode the fadec-instruction
	fadec::Instruction entry{};
	if (fadec::decode(reinterpret_cast<uint8_t*>(addr), _elf.get_size(addr), DecodeMode::decode_64, addr,
					  entry) <= 0) {
		inst_vec.~vector();
		Dispatcher::fault_exit("Failed to decode the instruction");
	}

	// transform the instruction into our representation
	auto inst_object = transform_instruction(entry);
	if (!inst_object) {
		// build the exception message
		const auto message = "Unsupported instruction used. ";
		const auto message_size = strlen(message);

		char exception_buffer[512];
		memcpy(exception_buffer, message, message_size);
		fadec::format(entry, exception_buffer + message_size, sizeof(exception_buffer) - message_size);

		inst_vec.~vector();
		Dispatcher::fault_exit(exception_buffer);
	}

	// adjust the address and add the instruction to the vector
	addr += inst_object->get_size();
	inst_vec.emplace_back(std::move(inst_object));

	return *inst_vec.back();
}

size_t CodeGenerator::recursive_flag_requirements(size_t& unclear, uintptr_t addr, uint8_t depth) const {
	// check if the instruction ends the recursion
	if (depth == 0)
		return unclear;

	//printf("rec_flag: %lx req: %lx dep: %u\n", addr, unclear, depth);

	// iterate through the addresses and decode the instructions
	size_t must_update = 0;
	while (true) {
		// decode the instruction
		fadec::Instruction entry{};
		if (fadec::decode(reinterpret_cast<uint8_t*>(addr), _elf.get_size(addr), DecodeMode::decode_64, addr, entry) <= 0)
			return unclear | must_update;

		// transform the instruction into our representation
		auto inst = transform_instruction(entry);
		if (!inst)
			return unclear | must_update;

		// adjust the address
		addr += inst->get_size();

		// update the required flags of the instruction
		must_update |= inst->get_require();
		if(const uintptr_t rec_addr = inst->recursive_require(); rec_addr != 0) {
			size_t unclear_branch = unclear & ~inst->get_update();
			must_update |= recursive_flag_requirements(unclear_branch, rec_addr, depth - 1);
			unclear &= unclear_branch;
		}

		unclear &= ~(inst->get_update() | must_update);

		//printf("\tunclear: %lx must_update: %lx\n", unclear, must_update);

		if (unclear == 0)
			return must_update;
		if (inst->get_eob())
			break;
	}
	return unclear | must_update;
}