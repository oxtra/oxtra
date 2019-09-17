#ifndef OXTRA_JNC_H
#define OXTRA_JNC_H

namespace codegen {
	class Jnc : public Jcc {
	public:
		explicit Jnc(const fadec::Instruction& inst)
				: Jcc{inst, flags::carry} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JNC_H
