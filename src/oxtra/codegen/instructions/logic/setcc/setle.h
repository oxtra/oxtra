#ifndef OXTRA_SETLE_H
#define OXTRA_SETLE_H

namespace codegen {
	class Setle : public Setcc {
	public:
		explicit Setle(const fadec::Instruction& inst)
				: Setcc{inst, flags::zero | flags::sign | flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETLE_H
