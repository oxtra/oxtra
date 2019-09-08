#ifndef OXTRA_IDIV_H
#define OXTRA_IDIV_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Idiv : public codegen::Instruction {
	public:
		explicit Idiv(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::all, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_IDIV_H
