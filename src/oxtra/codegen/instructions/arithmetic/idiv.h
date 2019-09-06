#ifndef OXTRA_IDIV_H
#define OXTRA_IDIV_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Idiv : public codegen::Instruction {
	public:
		explicit Idiv(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::all, Flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_IDIV_H
