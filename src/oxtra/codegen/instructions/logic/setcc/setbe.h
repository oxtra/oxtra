#ifndef OXTRA_SETBE_H
#define OXTRA_SETBE_H

namespace codegen {
	class Setbe : public Setcc {
	public:
		explicit Setbe(const fadec::Instruction& inst)
				: Setcc{inst, flags::carry | flags::zero} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETBE_H
