#ifndef OXTRA_JP_H
#define OXTRA_JP_H

namespace codegen {
	class Jp : public Jcc {
	public:
		explicit Jp(const fadec::Instruction& inst)
				: Jcc{inst, flags::parity} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_JP_H
