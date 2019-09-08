#ifndef OXTRA_SHL_H
#define OXTRA_SHL_H

#include "oxtra/codegen/instructions/binary_operation.h"

namespace codegen {
	class Shl : public BinaryOperation {
	public:
		explicit Shl(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::all, flags::none, true} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const override;
	};
}

#endif //OXTRA_SHL_H
