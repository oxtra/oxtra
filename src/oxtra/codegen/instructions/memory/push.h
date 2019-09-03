#ifndef OXTRA_PUSH_H
#define OXTRA_PUSH_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Push : public codegen::Instruction {
	public:
		explicit Push(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_PUSH_H
