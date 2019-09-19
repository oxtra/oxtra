#ifndef OXTRA_SETNP_H
#define OXTRA_SETNP_H

namespace codegen {
	class Setnp : public Setcc {
	public:
		explicit Setnp(const fadec::Instruction& inst)
				: Setcc{inst, flags::parity} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETNP_H
