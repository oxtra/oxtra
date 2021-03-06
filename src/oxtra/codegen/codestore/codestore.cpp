#include "codestore.h"
#include "oxtra/dispatcher/dispatcher.h"
#include "oxtra/logger/logger.h"

using namespace utils;
using namespace codegen::codestore;

CodeStore::CodeStore(const arguments::Arguments& args, const elf::Elf& elf)
		: _args{args}, _elf{elf}, _pages{elf.get_image_size() >> page_shift},
		  _code_buffer{args.get_instruction_list_size()}, _block_entries{args.get_entry_list_size()},
		  _instruction_offset_buffer{args.get_offset_list_size()} {}

host_addr_t CodeStore::find(guest_addr_t x86_code) const {
	// Index into the page table by shifting the address.
	auto&& entries = _pages[get_page_index(x86_code)];

	// Binary search through all basic block entries to find the one that contains the x86_code.
	for (size_t l = 0, r = entries.size(); l != r;) {
		const auto index = (l + r) / 2;
		const auto entry = entries[index];
		if (x86_code < entry->x86_start) {
			r = index;
		} else if (x86_code >= entry->x86_end) {
			l = index + 1;
		} else /*if (entry contains the address)*/ {
			// Find the address of the instructions corresponding to the x86_code.
			auto guest_address = entry->x86_start;
			auto host_address = entry->riscv_start;

			for (size_t i = 0; i < entry->instruction_count; ++i) {
				if (guest_address == x86_code)
					return host_address;

				auto&& instruction_entry = entry->offsets[i];
				guest_address += instruction_entry.x86;
				host_address += instruction_entry.riscv;
			}

			/*
			 * We know that the x86_code points inside this block, but we couldn't find an instruction corresponding to the code.
			 * That means that the x86_code points inside of an instruction. We could just start translating the block from that point onwards,
			 * but usually only obfuscated programs require this.
			 */
			dispatcher::Dispatcher::fault_exit("Jump inside instruction.");
		}
	}
	return 0;
}

BlockEntry* CodeStore::get_next_block(guest_addr_t x86_code) const {
	// std::span in c++20 c:
	for (auto index = get_page_index(x86_code); index < _pages.size(); ++index) {
		auto&& entries = _pages[index];

		/* Find the next block, who's start-address is larger or equal to the address to query, and return it.
		 * This is only valid, if we make sure that the entries within one page are sorted ascending by
		 * their address. */
		for (const auto entry : entries) {
			if (entry->x86_start >= x86_code)
				return entry;
		}
	}
	return nullptr;
}

BlockEntry& CodeStore::create_block() {
	return _block_entries.allocate_entry();
}

void CodeStore::add_instruction(BlockEntry& block, utils::guest_addr_t address, uint8_t size,
								riscv_instruction_t* riscv_instructions, size_t num_instructions) {
	insert_block(block, address);

	block.riscv_start = reinterpret_cast<host_addr_t>(
			_code_buffer.add(reinterpret_cast<riscv_instruction_t*>(block.riscv_start), riscv_instructions,
							 num_instructions));
	
	if (!block.riscv_start)
		dispatcher::Dispatcher::fault_exit("failed to add the riscv instructions to the static list");

	block.offsets = _instruction_offset_buffer.add(block.offsets, {size, static_cast<uint16_t>(num_instructions * 4)});

	if (!block.offsets)
		dispatcher::Dispatcher::fault_exit("failed to add riscv instruction sizes (i.e. offsets) to the static list");

	block.instruction_count++;
	block.x86_end = address + size;

	// check if the block overlaps into another page
	if ((block.x86_start >> page_shift) < (block.x86_end >> page_shift)) {
		/* This block will be executed for every instruction added, which lies within another page.
		 * As we expect instruction-blocks not to overlap, we can accelerate the execution, by making the assumption
		 * that the block has not been added to the new page yet, if the first entry of the overlapping page-array is
		 * not equal to this block. This assumption should hold true, as the new block is coming from the lower
		 * addresses and thus via the base-address of the page. */
		auto& page_array = _pages[get_page_index(block.x86_end)];
		if (page_array.empty()) {
			page_array.push_back(&block);
		} else if (page_array[0] != &block) {
			page_array.insert(page_array.begin(), &block);
		}
	}
}

void CodeStore::insert_block(codegen::codestore::BlockEntry& block, utils::guest_addr_t x86_address) {
	// if there's no x86 instruction then this is the first instruction to add to the block
	if (block.instruction_count == 0) {
		block.x86_start = x86_address;

		// sort the block by its address into the current page (ascending)
		auto&& page_array = _pages[get_page_index(block.x86_start)];
		for (size_t i = 0; i < page_array.size(); i++) {
			if (page_array[i]->x86_start > x86_address) {
				page_array.insert(page_array.begin() + i, &block);
				return;
			}
		}

		page_array.push_back(&block);

	} else if (block.x86_end != x86_address)
		dispatcher::Dispatcher::fault_exit("Tried to add a non-consecutive instruction to a block.");

}

size_t CodeStore::get_page_index(utils::guest_addr_t x86_address) const {
	return (x86_address - _elf.get_base_vaddr()) >> page_shift;
}