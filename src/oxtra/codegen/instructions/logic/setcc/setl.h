#ifndef OXTRA_SETL_H
#define OXTRA_SETL_H

namespace codegen {
	class Setl : public Setcc {
	public:
		explicit Setl(const fadec::Instruction& inst)
				: Setcc{inst, flags::sign | flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETL_H
