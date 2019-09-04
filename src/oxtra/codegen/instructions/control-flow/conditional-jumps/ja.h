#ifndef OXTRA_JA_H
#define OXTRA_JA_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Ja : public codegen::Instruction {
	public:
		explicit Ja(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::none, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JA_H
