#ifndef OXTRA_ROL_H
#define OXTRA_ROL_H

namespace codegen {
	class RolImm : public Rotation {
	public:
		explicit RolImm(const fadec::Instruction& inst)
			: Rotation{inst} {}

		encoding::RiscVRegister rotate(CodeBatch& batch, encoding::RiscVRegister reg) const final;
	};

	class RolCl : public Rotation {
	public:
		explicit RolCl(const fadec::Instruction& inst)
			: Rotation{inst} {}

		encoding::RiscVRegister rotate(CodeBatch& batch, encoding::RiscVRegister reg) const final;
	};
}

#endif //OXTRA_ROL_H
