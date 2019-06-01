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

	struct InstOffset {
		uint8_t _x86 : 4;
		uint8_t _riscv : 4;
	};
	struct BlockEntry {
		oxtra::virt_t _virt;
		oxtra::virt_t _last;
		oxtra::real_t _real;
		size_t _inst_count;
		InstOffset *_offset;
	};
	struct BlockArray {
		BlockEntry *_entry;
		size_t _capacity;
		size_t _size;
	};

	class CodeStore {

	};
}

#endif //OXTRA_CODESTORE_H
