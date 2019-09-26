#ifndef OXTRA_ROR_H
#define OXTRA_ROR_H

namespace codegen {
	class RorImm : public Rotation {
	public:
		explicit RorImm(const fadec::Instruction& inst)
				: Rotation{inst} {}

		encoding::RiscVRegister rotate(CodeBatch& batch, encoding::RiscVRegister reg) const final;
	};

	class RorCl : public Rotation {
	public:
		explicit RorCl(const fadec::Instruction& inst)
				: Rotation{inst} {}

		encoding::RiscVRegister rotate(CodeBatch& batch, encoding::RiscVRegister reg) const final;
	};
}


#endif //OXTRA_ROR_H
