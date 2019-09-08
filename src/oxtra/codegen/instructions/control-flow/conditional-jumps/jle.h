#ifndef OXTRA_JAE_H
#define OXTRA_JAE_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jle : public codegen::Instruction {
	public:
		explicit Jle(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::zero | flags::sign | flags::overflow, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JAE_H
