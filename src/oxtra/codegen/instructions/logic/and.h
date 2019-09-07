#ifndef OXTRA_AND_H
#define OXTRA_AND_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class And : public BinaryOperation {
	public:
		explicit And(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::all, flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_AND_H
