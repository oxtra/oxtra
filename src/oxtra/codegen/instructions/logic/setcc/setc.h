#ifndef OXTRA_SETC_H
#define OXTRA_SETC_H

namespace codegen {
	class Setc : public Setcc {
	public:
		explicit Setc(const fadec::Instruction& inst)
				: Setcc{inst, flags::carry} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETC_H
