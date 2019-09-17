#ifndef OXTRA_JCXZ_H
#define OXTRA_JCXZ_H

namespace codegen {
	class Jcxz : public Jcc {
	public:
		explicit Jcxz(const fadec::Instruction& inst)
				: Jcc{inst, flags::none} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JCXZ_H
