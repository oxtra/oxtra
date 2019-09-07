#ifndef OXTRA_JMP_H
#define OXTRA_JMP_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jmp : public codegen::Instruction {
	public:
		explicit Jmp(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::none, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JMP_H
