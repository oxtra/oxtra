#ifndef OXTRA_JP_H
#define OXTRA_JP_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jp : public codegen::Instruction {
	public:
		explicit Jp(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::parity, true} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_JP_H
