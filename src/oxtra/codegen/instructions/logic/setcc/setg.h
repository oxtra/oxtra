#ifndef OXTRA_SETG_H
#define OXTRA_SETG_H

namespace codegen {
	class Setg : public Setcc {
	public:
		explicit Setg(const fadec::Instruction& inst)
				: Setcc{inst, flags::zero | flags::sign | flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETG_H
