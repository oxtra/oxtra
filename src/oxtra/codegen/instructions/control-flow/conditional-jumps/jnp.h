#ifndef OXTRA_JNP_H
#define OXTRA_JNP_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jnp : public codegen::Instruction {
	public:
		explicit Jnp(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::parity, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JNP_H
