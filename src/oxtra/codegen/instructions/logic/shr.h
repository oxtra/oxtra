#ifndef OXTRA_SHR_H
#define OXTRA_SHR_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Shr : public BinaryOperation {
	public:
		explicit Shr(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_SHR_H
