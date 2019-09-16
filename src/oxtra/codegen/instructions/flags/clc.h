#ifndef OXTRA_CLC_H
#define OXTRA_CLC_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Clc : public codegen::Instruction {
	public:
		explicit Clc(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::carry, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_CLC_H
