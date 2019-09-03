#ifndef OXTRA_CALL_H
#define OXTRA_CALL_H

#include "oxtra/codegen/instruction.h"
#include "jmp.h"

namespace codegen {
	class Call : public Jmp {
	public:
		explicit Call(const fadec::Instruction& inst)
				: codegen::Jmp{inst} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_CALL_H
