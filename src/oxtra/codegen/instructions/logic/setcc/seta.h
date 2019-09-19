#ifndef OXTRA_SETA_H
#define OXTRA_SETA_H

namespace codegen {
	class Seta : public Setcc {
	public:
		explicit Seta(const fadec::Instruction& inst)
			: Setcc{inst, flags::carry | flags::zero} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETA_H
