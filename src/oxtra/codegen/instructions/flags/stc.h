#ifndef OXTRA_STC_H
#define OXTRA_STC_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Stc : public codegen::Instruction {
	public:
		explicit Stc(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::carry, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}
#endif //OXTRA_STC_H
