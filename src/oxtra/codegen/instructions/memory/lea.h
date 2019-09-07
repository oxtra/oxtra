#ifndef OXTRA_LEA_H
#define OXTRA_LEA_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Lea : public codegen::Instruction {
	public:
		explicit Lea(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}
#endif //OXTRA_LEA_H
