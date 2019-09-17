#ifndef OXTRA_JAE_H
#define OXTRA_JAE_H

namespace codegen {
	class Jle : public Jcc {
	public:
		explicit Jle(const fadec::Instruction& inst)
				: Jcc{inst, flags::zero | flags::sign | flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JAE_H
