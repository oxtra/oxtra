#ifndef OXTRA_CODESTORE_H
#define OXTRA_CODESTORE_H

#include "oxtra/types.h"

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

	 /*
	 look at VADs from windows
	 */

	struct InstOffset {
		uint8_t x86 : 4;
		uint8_t riscv : 4;
	};

	struct BlockEntry {
		oxtra::virt_t virt;
		oxtra::virt_t last;
		oxtra::real_t real;
		size_t inst_count;
		InstOffset *offset;
	};

	class CodeStore {
	private:

		/** stores which block entries are in a 'page' */
		using BlockArray = std::vector<BlockEntry*>;

		std::unique_ptr<BlockArray[]> _pages;

		/** risc-v code buffer */
		uint8_t* _code_buffer;
		size_t _size_left;
		const size_t _buffer_size;

		/** block entry buffer */
		BlockEntry* _entry_buffer;
		size_t _entries_left;

	public:
		CodeStore(const elf::Elf& elf);

		oxtra::real_t find(oxtra::virt_t x86code) const;

		oxtra::real_t get_next_block(oxtra::virt_t& x86code) const;

		void add_instructions(BlockEntry& block, );

		uint8_t* get_code_buffer() const;

		size_t get_code_buffer_size() const;

		void* resize_code_buffer(size_t& size);
	};
}

#endif //OXTRA_CODESTORE_H
