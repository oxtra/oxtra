#ifndef OXTRA_JZ_H
#define OXTRA_JZ_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jz : public codegen::Instruction {
	public:
		explicit Jz(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::zero, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JZ_H
