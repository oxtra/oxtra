#ifndef OXTRA_JL_H
#define OXTRA_JL_H

namespace codegen {
	class Jl : public Jcc {
	public:
		explicit Jl(const fadec::Instruction& inst)
				: Jcc{inst, flags::sign | flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JL_H
