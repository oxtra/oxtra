#ifndef OXTRA_NOT_H
#define OXTRA_NOT_H

#include "oxtra/codegen/unary_operation.h"

namespace codegen {
	class Not : public UnaryOperation {
	public:
		explicit Not(const fadec::Instruction& inst)
				: UnaryOperation{inst, Flags::none, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const override;
	};
}

#endif //OXTRA_NOT_H