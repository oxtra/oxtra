#ifndef OXTRA_MOVSX_H
#define OXTRA_MOVSX_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Movsx : public BinaryOperation {
	public:
		explicit Movsx(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::none, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_MOVSX_H