#ifndef OXTRA_JG_H
#define OXTRA_JG_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jg : public codegen::Instruction {
	public:
		explicit Jg(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::zero | flags::sign | flags::overflow, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JG_H
