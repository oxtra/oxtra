#ifndef OXTRA_SETNZ_H
#define OXTRA_SETNZ_H

namespace codegen {
	class Setnz : public Setcc {
	public:
		explicit Setnz(const fadec::Instruction& inst)
				: Setcc{inst, flags::zero} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETNZ_H
