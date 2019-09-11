#ifndef OXTRA_MUL_H
#define OXTRA_MUL_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Mul : public codegen::Instruction {
	public:
		explicit Mul(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::all, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_MUL_H
