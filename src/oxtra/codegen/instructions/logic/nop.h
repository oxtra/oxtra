#ifndef OXTRA_NOP_H
#define OXTRA_NOP_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Nop : public codegen::Instruction {
	public:
		explicit Nop(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_NOP_H
