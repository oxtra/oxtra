#ifndef OXTRA_SHR_H
#define OXTRA_SHR_H

namespace codegen {
	class Shr : public BinaryOperation {
	public:
		explicit Shr(const fadec::Instruction& inst)
				: BinaryOperation{inst, flags::all, flags::none, true} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const override;
	};

	class ShrCl : public ShiftCl {
	public:
		explicit ShrCl(const fadec::Instruction& inst)
				: ShiftCl{inst} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const final;
	};
}

#endif //OXTRA_SHR_H
