#ifndef OXTRA_JZ_H
#define OXTRA_JZ_H

namespace codegen {
	class Jz : public Jcc {
	public:
		explicit Jz(const fadec::Instruction& inst)
				: Jcc{inst, flags::zero} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JZ_H
