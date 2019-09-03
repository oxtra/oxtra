#ifndef OXTRA_SAR_H
#define OXTRA_SAR_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Sar : public BinaryOperation {
	public:
		explicit Sar(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_SAR_H
