#ifndef OXTRA_JG_H
#define OXTRA_JG_H

namespace codegen {
	class Jg : public Jcc {
	public:
		explicit Jg(const fadec::Instruction& inst)
				: Jcc{inst, flags::zero | flags::sign | flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JG_H
