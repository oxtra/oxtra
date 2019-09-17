#ifndef OXTRA_JC_H
#define OXTRA_JC_H

namespace codegen {
	class Jc : public Jcc {
	public:
		explicit Jc(const fadec::Instruction& inst)
				: Jcc{inst, flags::carry} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JC_H
