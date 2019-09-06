#ifndef OXTRA_JNZ_H
#define OXTRA_JNZ_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jnz : public codegen::Instruction {
	public:
		explicit Jnz(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::zero, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JNZ_H
