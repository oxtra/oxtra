#ifndef OXTRA_SHL_H
#define OXTRA_SHL_H

namespace codegen {
	class Shl : public BinaryOperation {
	public:
		explicit Shl(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::all, flags::none, true} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const override;
	};

	class ShlCl : public ShiftCl {
	public:
		explicit ShlCl(const fadec::Instruction& inst)
				: ShiftCl{inst} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const final;
	};
}

#endif //OXTRA_SHL_H
