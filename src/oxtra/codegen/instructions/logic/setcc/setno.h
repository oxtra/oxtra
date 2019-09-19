#ifndef OXTRA_SETNO_H
#define OXTRA_SETNO_H

namespace codegen {
	class Setno : public Setcc {
	public:
		explicit Setno(const fadec::Instruction& inst)
				: Setcc{inst, flags::overflow} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETNO_H
