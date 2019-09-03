#ifndef OXTRA_IMUL_H
#define OXTRA_IMUL_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Imul : public BinaryOperation {
	public:
		explicit Imul(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_IMUL_H
