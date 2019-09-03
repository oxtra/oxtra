#ifndef OXTRA_SYSCALL_H
#define OXTRA_SYSCALL_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Syscall : public codegen::Instruction {
	public:
		explicit Syscall(const fadec::Instruction& inst)
			: codegen::Instruction{inst, Flags::all, Flags::none, true} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SYSCALL_H
