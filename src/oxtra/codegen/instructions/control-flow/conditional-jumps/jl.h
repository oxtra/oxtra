#ifndef OXTRA_JL_H
#define OXTRA_JL_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jl : public codegen::Instruction {
	public:
		explicit Jl(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::sign | flags::overflow, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JL_H
