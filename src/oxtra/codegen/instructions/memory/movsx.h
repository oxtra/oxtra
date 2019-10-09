#ifndef OXTRA_MOVSX_H
#define OXTRA_MOVSX_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Movsx : public Instruction {
	public:
		explicit Movsx(const fadec::Instruction& inst)
				: Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_MOVSX_H
