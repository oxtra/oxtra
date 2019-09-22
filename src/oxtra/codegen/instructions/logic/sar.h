#ifndef OXTRA_SAR_H
#define OXTRA_SAR_H

#include "oxtra/codegen/instructions/binary_operation.h"

namespace codegen {
	class Sar : public BinaryOperation {
	public:
		explicit Sar(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::all, flags::none, true} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const override;
	};

	class SarCl : public ShiftCl {
	public:
		explicit SarCl(const fadec::Instruction& inst)
			: ShiftCl{inst} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const final;
	};
}

#endif //OXTRA_SAR_H
