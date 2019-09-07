#ifndef OXTRA_XOR_H
#define OXTRA_XOR_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Xor : public BinaryOperation {
	public:
		explicit Xor(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::all, flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}

#endif //OXTRA_XOR_H
