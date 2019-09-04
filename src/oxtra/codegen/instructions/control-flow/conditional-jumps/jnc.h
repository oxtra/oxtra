#ifndef OXTRA_JNC_H
#define OXTRA_JNC_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jnc : public codegen::Instruction {
	public:
		explicit Jnc(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::carry, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JNC_H
