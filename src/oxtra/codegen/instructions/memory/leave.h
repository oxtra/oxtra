#ifndef OXTRA_LEAVE_H
#define OXTRA_LEAVE_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Leave : public codegen::Instruction {
	public:
		explicit Leave(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_LEAVE_H
