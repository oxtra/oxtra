#ifndef OXTRA_INC_H
#define OXTRA_INC_H

#include "oxtra/codegen/unary_operation.h"

namespace codegen {
	class Inc : public UnaryOperation {
	public:
		explicit Inc(const fadec::Instruction& inst)
				: UnaryOperation{inst, Flags::all ^ Flags::carry, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const override;
	};
}

#endif //OXTRA_INC_H
