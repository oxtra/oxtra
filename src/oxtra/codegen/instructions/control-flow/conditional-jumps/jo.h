#ifndef OXTRA_JO_H
#define OXTRA_JO_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jo : public codegen::Instruction {
	public:
		explicit Jo(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::overflow, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JO_H
