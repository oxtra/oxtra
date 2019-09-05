#ifndef OXTRA_JC_H
#define OXTRA_JC_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jc : public codegen::Instruction {
	public:
		explicit Jc(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::carry, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JC_H
