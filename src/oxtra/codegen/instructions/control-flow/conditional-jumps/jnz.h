#ifndef OXTRA_JNZ_H
#define OXTRA_JNZ_H

namespace codegen {
	class Jnz : public Jcc {
	public:
		explicit Jnz(const fadec::Instruction& inst)
				: Jcc{inst, flags::zero} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JNZ_H
