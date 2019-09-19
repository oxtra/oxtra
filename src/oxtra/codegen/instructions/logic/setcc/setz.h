#ifndef OXTRA_SETZ_H
#define OXTRA_SETZ_H

namespace codegen {
	class Setz : public Setcc {
	public:
		explicit Setz(const fadec::Instruction& inst)
				: Setcc{inst, flags::zero} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETZ_H
