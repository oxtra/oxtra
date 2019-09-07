#ifndef OXTRA_JGE_H
#define OXTRA_JGE_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jge : public codegen::Instruction {
	public:
		explicit Jge(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::sign | flags::overflow, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JGE_H
