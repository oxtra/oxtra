#ifndef OXTRA_JNS_H
#define OXTRA_JNS_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jns : public codegen::Instruction {
	public:
		explicit Jns(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::sign, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JNS_H
