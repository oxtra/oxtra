#ifndef OXTRA_POP_H
#define OXTRA_POP_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Pop : public codegen::Instruction {
	public:
		explicit Pop(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_POP_H
