#ifndef OXTRA_AND_H
#define OXTRA_AND_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class And : public BinaryOperation {
	public:
		explicit And(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none, true} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const override;
	};
}

#endif //OXTRA_AND_H
