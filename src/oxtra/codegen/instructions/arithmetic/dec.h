#ifndef OXTRA_DEC_H
#define OXTRA_DEC_H

#include "oxtra/codegen/unary_operation.h"

namespace codegen {
	class Dec : public UnaryOperation {
	public:
		explicit Dec(const fadec::Instruction& inst)
				: UnaryOperation{inst, Flags::all ^ Flags::carry, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const override;
	};
}

#endif //OXTRA_DEC_H
