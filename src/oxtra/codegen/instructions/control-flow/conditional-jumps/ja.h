#ifndef OXTRA_JA_H
#define OXTRA_JA_H

namespace codegen {
	class Ja : public Jcc {
	public:
		explicit Ja(const fadec::Instruction& inst)
				: Jcc{inst, flags::carry | flags::zero} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JA_H
