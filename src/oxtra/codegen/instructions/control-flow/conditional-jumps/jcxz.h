#ifndef OXTRA_JCXZ_H
#define OXTRA_JCXZ_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jcxz : public codegen::Instruction {
	public:
		explicit Jcxz(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::none, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JCXZ_H
