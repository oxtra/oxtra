#ifndef OXTRA_JS_H
#define OXTRA_JS_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Js : public codegen::Instruction {
	public:
		explicit Js(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::sign, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JS_H
