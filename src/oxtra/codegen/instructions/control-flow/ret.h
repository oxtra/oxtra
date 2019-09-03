#ifndef OXTRA_RET_H
#define OXTRA_RET_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Ret : public codegen::Instruction {
	public:
		explicit Ret(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_RET_H
