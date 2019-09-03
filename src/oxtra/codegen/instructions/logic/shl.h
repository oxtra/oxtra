#ifndef OXTRA_SHL_H
#define OXTRA_SHL_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Shl : public BinaryOperation {
	public:
		explicit Shl(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_SHL_H
