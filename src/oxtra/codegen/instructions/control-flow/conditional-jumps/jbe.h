#ifndef OXTRA_JBE_H
#define OXTRA_JBE_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jbe : public codegen::Instruction {
	public:
		explicit Jbe(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::carry | flags::zero, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JA_H
