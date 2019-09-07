#ifndef OXTRA_MOVZX_H
#define OXTRA_MOVZX_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Movzx : public BinaryOperation {
	public:
		explicit Movzx(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::none, flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_MOVZX_H
