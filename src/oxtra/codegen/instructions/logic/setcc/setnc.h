#ifndef OXTRA_SETNC_H
#define OXTRA_SETNC_H

namespace codegen {
	class Setnc : public Setcc {
	public:
		explicit Setnc(const fadec::Instruction& inst)
				: Setcc{inst, flags::carry} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETNC_H
