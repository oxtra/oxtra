#ifndef OXTRA_MOV_H
#define OXTRA_MOV_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Mov : public codegen::Instruction {
	public:
		explicit Mov(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_MOV_H
