#include "oxtra/codegen/codegen.h"
#include "oxtra/codegen/Test/transform_instruction.h"
#include "oxtra/dispatcher/dispatcher.h"
#include "helper.h"
#include <spdlog/spdlog.h>

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
				instructions[instructions.size() - 1]->set_eob();
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

	// iterate through the instructions backwards and check where the instructions have to be up-to-date
	size_t required_updates = 0;
	for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
		// get the instruction
		auto&& inst = *it;

		// extract the flags the instruction has to update
		size_t need_update = inst->get_update() & required_updates;
		required_updates ^= need_update;

		// add the requirements of this instruction to the search-requirements,
		// to indicate to previous instructions, that the flags are needed, and update its update-flags
		required_updates |= inst->get_require();
		inst->set_update(need_update);
	}

	// iterate through the instructions and translate them to riscv-code
	// TODO: instatiate code batch here based on the debug settings
	CodeStash batch{};
	auto&& codeblock = _codestore.create_block();
	for (size_t i = 0; i < instructions.size(); i++) {
		auto&& inst = instructions[i];
		batch.reset();

		// translate the instruction
		inst->generate(batch);

		// check if the instruction is the last instruction and the upcoming block is known
		if (next_block) {
			if (i == instructions.size() - 1) {
				// append the forcing connection to the next block
				helper::load_immediate(batch, next_block->riscv_start, helper::address_destination);
				batch += encoding::JALR(RiscVRegister::zero, helper::address_destination, 0);
			}
		}

		batch.end();

#ifdef DEBUG
		spdlog::debug("decoded {}", inst->string());
		spdlog::trace(batch.string());
#endif

		// add the instruction to the store
		_codestore.add_instruction(codeblock, inst->get_address(), inst->get_size(), batch.get(), batch.size());
	}

	// add dynamic tracing-information for the basic-block
	spdlog::info("basicblock translated: x86: [0x{0:x} - 0x{1:x}] riscv: 0x{2:x}", codeblock.x86_start, codeblock.x86_end,
				 codeblock.riscv_start);

	return codeblock.riscv_start;
}

void CodeGenerator::update_basic_block(utils::host_addr_t addr, utils::host_addr_t absolute_address) {
	// compute new base-address where the new absolute address will be written to t0
	// 9 = 8 [load_64bit_immediate] + 1 [JALR]
	CodeMemory code{reinterpret_cast<riscv_instruction_t*>(addr - 9 * sizeof(riscv_instruction_t))};

	// write the new instructions
	helper::load_address(code, absolute_address, helper::address_destination);
#ifdef DEBUG
	if (code.size() != 8)
		Dispatcher::fault_exit("load_64bit_immediate did not generate 8 instructions");
#endif
	code += encoding::JALR(RiscVRegister::zero, helper::address_destination, 0);
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
	inst_vec.push_back(std::move(inst_object));

	return *inst_vec.back();
}