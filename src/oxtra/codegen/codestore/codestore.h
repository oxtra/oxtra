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

	/*
	 *  xxxxvvvv
	 *
	 *	//x86
	 *	inst[0] = _virt;
	 *	inst[1] = inst[0] + (_offset[0] >> 4);
	 *  inst[2] = inst[1] + (_offset[1] >> 4);
	 * 	inst[3] = inst[2] + (_offset[2] >> 4);
	 *
	 *
	 *	//riscv
	 *	inst[0] = _real;
	 *	inst[1] = inst[0] + ((_offset[0] & 0x0f) << 2);
	 *  inst[2] = inst[1] + ((_offset[1] & 0x0f) << 2);
	 * 	inst[3] = inst[2] + ((_offset[2] & 0x0f) << 2);
	 *
	 */

	struct InstructionOffset {
		/**
		 * number of bytes for the x86 instruction
		 */
		uint8_t x86 : 4;

		/**
		 * number of risc-v instructions (every instruction is 4 bytes)
		 */
		uint8_t riscv : 4;
	};

	struct BlockEntry {
		utils::guest_addr_t x86_start;
		utils::guest_addr_t x86_end;
		utils::host_addr_t riscv_start;
		size_t instruction_count;
		const InstructionOffset* offsets;
	};

	class CodeStore {
	private:
		static constexpr size_t
			page_shift = 12,
			page_size = (1 << page_shift);

		/** stores which block entries are in a 'page'*/
		using BlockArray = std::vector<BlockEntry*>;

		utils::FixedArray<BlockArray> _pages;

		/** risc-v code buffer */
		utils::StaticList<utils::riscv_instruction_t> _code_buffer;

		/** block entry buffer */
		utils::StaticList<BlockEntry> _block_entries;

		/** global instruction offset buffer */
		utils::StaticList<InstructionOffset> _instruction_offset_buffer;

		/** elf-image object */
		const elf::Elf& _elf;

		/** arguments-parser object */
		const arguments::Arguments& _args;

	public:
		CodeStore(const elf::Elf& elf, const arguments::Arguments& args);

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
	};
}

#endif //OXTRA_CODESTORE_H
