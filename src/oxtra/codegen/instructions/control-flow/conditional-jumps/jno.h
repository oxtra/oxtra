#ifndef OXTRA_JNO_H
#define OXTRA_JNO_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jno : public codegen::Instruction {
	public:
		explicit Jno(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::overflow, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JNO_H
