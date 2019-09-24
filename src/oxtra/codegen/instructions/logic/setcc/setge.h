#ifndef OXTRA_SETGE_H
#define OXTRA_SETGE_H

namespace codegen {
	class Setge : public Setcc {
	public:
		explicit Setge(const fadec::Instruction& inst)
				: Setcc{inst, flags::sign | flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETGE_H
