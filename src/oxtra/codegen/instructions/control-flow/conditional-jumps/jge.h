#ifndef OXTRA_JGE_H
#define OXTRA_JGE_H

namespace codegen {
	class Jge : public Jcc {
	public:
		explicit Jge(const fadec::Instruction& inst)
				: Jcc{inst, flags::sign | flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JGE_H
