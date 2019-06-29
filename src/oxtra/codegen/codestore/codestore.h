#ifndef OXTRA_CODESTORE_H
#define OXTRA_CODESTORE_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/utils/static_list.h"
#include "oxtra/utils/fixed_array.h"
#include "oxtra/elf/elf.h"
#include <vector>
#include <memory>
#include <fadec.h>

namespace codegen::codestore {

	constexpr size_t max_riscv_instructions_bits = 10;
	constexpr size_t max_riscv_instructions = (0x01u << max_riscv_instructions_bits) - 1;

	struct InstructionOffset {
		/**
		 * number of bytes for the x86 instruction
		 */
		uint16_t x86 : 6;

		/**
		 * number of risc-v instructions (every instruction is 4 bytes)
		 */
		uint16_t riscv : max_riscv_instructions_bits;
	};

	struct BlockEntry {
		utils::guest_addr_t x86_start = 0;
		utils::guest_addr_t x86_end = 0;
		utils::host_addr_t riscv_start = 0;
		size_t instruction_count = 0;
		const InstructionOffset* offsets = nullptr;
	};

	class CodeStore {
	private:
		/** stores which block entries are in a 'page' */
		using BlockArray = std::vector<BlockEntry*>;

		static constexpr size_t
				page_shift = 12,
				page_size = (1 << page_shift);

		/** arguments-parser object */
		const arguments::Arguments& _args;

		/** elf-image object */
		const elf::Elf& _elf;

		utils::FixedArray<BlockArray> _pages;

		/** risc-v code buffer */
		utils::ExecutableList<utils::riscv_instruction_t> _code_buffer;

		/** block entry buffer */
		utils::StaticList<BlockEntry> _block_entries;

		/** global instruction offset buffer */
		utils::StaticList<InstructionOffset> _instruction_offset_buffer;

	public:
		CodeStore(const arguments::Arguments& args, const elf::Elf& elf);

		/**
		 *
		 * @param x86_code The address of the x86 instructions.
		 * @return The address of the translated risc-v instructions. May be null, if not translated yet.
		 */
		utils::host_addr_t find(utils::guest_addr_t x86_code) const;

		/**
		 *
		 * @param x86_code The address of the x86 code used to find the next block.
		 * @return a pointer to the next translated block that may conflict with this x86_code-block. May be null.
		 */
		BlockEntry* get_next_block(utils::guest_addr_t x86_code) const;

		/**
		 * Allocate a new block entry.
		 * @return A newly allocated block entry.
		 */
		BlockEntry& create_block();

		/**
		 * Adds an x86 instruction with the corresponding risc-v instructions to a block.
		 * @param block The basic block that the x86 instruction belongs to.
		 * @param x86_instruction The decoded x86 instruction (contains address and size).
		 * @param riscv_instructions A pointer to an array of encoded risc-v instructions.
		 * @param num_instructions The size of the array of encoded risc-v instructions.
		 */
		void add_instruction(BlockEntry& block, const fadec::Instruction& x86_instruction,
							 utils::riscv_instruction_t* riscv_instructions, size_t num_instructions);

	private:
		void insert_block(BlockEntry& block, utils::guest_addr_t x86_address);

		size_t get_page_index(utils::guest_addr_t x86_address) const;
	};
}

#endif //OXTRA_CODESTORE_H
