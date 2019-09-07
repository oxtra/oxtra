#ifndef OXTRA_SHR_H
#define OXTRA_SHR_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Shr : public BinaryOperation {
	public:
		explicit Shr(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none, true} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const override;
	};
}

#endif //OXTRA_SHR_H
