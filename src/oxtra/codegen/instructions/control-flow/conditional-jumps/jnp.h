#ifndef OXTRA_JNP_H
#define OXTRA_JNP_H

namespace codegen {
	class Jnp : public Jcc {
	public:
		explicit Jnp(const fadec::Instruction& inst)
				: Jcc{inst, flags::parity} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JNP_H
