#include "oxtra/codegen/codestore/codestore.h"

#include <algorithm>

using namespace utils;
using namespace codegen::codestore;

CodeStore::CodeStore(const elf::Elf& elf)
		: _elf{elf}, _instruction_offset_buffer{0x10000}, _block_entries{0x10000}, _code_buffer{0x10000},
		_pages{elf.get_image_size() >> page_shift} {}

host_addr_t CodeStore::find(guest_addr_t x86_code) const {
	// Index into the page table by shifting the address.
	auto&& entries = _pages[x86_code >> page_shift];

	// Loop through all basic block entries to find the one that contains the x86_code.
	for (const auto entry : entries) {
		if (x86_code >= entry->x86_start && x86_code < entry->x86_end) {
			// Find the address of the instructions corresponding to the x86_code.
			auto guest_address = entry->x86_start;
			auto host_address = entry->riscv_start;

			for (size_t i = 0; i < entry->instruction_count; ++i) {
				if (guest_address == x86_code)
					return host_address;

				auto&& instruction_entry = entry->offsets[i];
				guest_address += instruction_entry.x86;
				host_address += instruction_entry.riscv * sizeof(riscv_instruction_t);
			}

			/*
			 * We know that the x86_code points inside this block, but we couldn't find an instruction corresponding to the code.
			 * That means that the x86_code points inside of an instruction. We could just start translating the block from that point onwards,
			 * but usually only obfuscated programs require this.
			 */
			throw new std::runtime_error("Jump inside instruction.");
		}
	}

	return 0;

}

BlockEntry* CodeStore::get_next_block(guest_addr_t x86_code) const {
	// std::span in c++20 c:
	for (auto index = x86_code >> page_shift; index < _pages.size(); ++index) {
		auto&& entries = _pages[index];

		BlockEntry* closest = nullptr;

		for (const auto entry : entries) {
			// Update closest if it's closer than the previously closest entry.
			if (entry->x86_start >= x86_code && (closest == nullptr || closest->x86_start > entry->x86_start))
				closest = entry;
		}

		// Did we find a block that comes after x86_code?
		if (closest)
			return closest;
	}

	return nullptr;
}

BlockEntry& CodeStore::create_block() {
	return _block_entries.allocate_entry();
}

void CodeStore::add_instruction(BlockEntry& block, const fadec::Instruction& x86_instruction,
								riscv_instruction_t* riscv_instructions, size_t num_instructions) {
	// If there's no x86 start address then this is the first instruction to add to the block.
	if (block.x86_start == 0) {
		block.x86_start = x86_instruction.get_address();
		_pages[block.x86_start >> page_shift].push_back(&block);
	}

	// Maybe do this for the debug build only?
	else if (block.x86_end != x86_instruction.get_address()) {
		throw new std::runtime_error("Tried to add a non-consecutive instruction to a block.");
	}

	block.riscv_start = reinterpret_cast<host_addr_t>(
			_code_buffer.add(reinterpret_cast<riscv_instruction_t*>(block.riscv_start), riscv_instructions, num_instructions));

	block.offsets = _instruction_offset_buffer.add(block.offsets, {x86_instruction.get_size(), static_cast<uint8_t>(num_instructions)});
	block.instruction_count++;
	block.x86_end = x86_instruction.get_address() + x86_instruction.get_size();
}