#ifndef OXTRA_NEG_H
#define OXTRA_NEG_H

#include "oxtra/codegen/unary_operation.h"

namespace codegen {
	class Neg : public UnaryOperation {
	public:
		explicit Neg(const fadec::Instruction& inst)
				: UnaryOperation{inst, flags::all, flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const override;
	};
}

#endif //OXTRA_NEG_H
