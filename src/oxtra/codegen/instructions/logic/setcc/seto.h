#ifndef OXTRA_SETO_H
#define OXTRA_SETO_H

namespace codegen {
	class Seto : public Setcc {
	public:
		explicit Seto(const fadec::Instruction& inst)
				: Setcc{inst, flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETO_H
