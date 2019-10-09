#ifndef OXTRA_MOVZX_H
#define OXTRA_MOVZX_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Movzx : public codegen::Instruction {
	public:
		explicit Movzx(const fadec::Instruction& inst)
				: Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_MOVZX_H
