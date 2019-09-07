#ifndef OXTRA_ADD_H
#define OXTRA_ADD_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Add : public BinaryOperation {
	public:
		explicit Add(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::all, flags::none, true} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const override;
	};
}

#endif //OXTRA_ADD_H
