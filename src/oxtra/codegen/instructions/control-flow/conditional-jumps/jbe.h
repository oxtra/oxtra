#ifndef OXTRA_JBE_H
#define OXTRA_JBE_H

namespace codegen {
	class Jbe : public Jcc {
	public:
		explicit Jbe(const fadec::Instruction& inst)
				: Jcc{inst, flags::carry | flags::zero} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JA_H
