#ifndef OXTRA_SETS_H
#define OXTRA_SETS_H

namespace codegen {
	class Sets : public Setcc {
	public:
		explicit Sets(const fadec::Instruction& inst)
				: Setcc{inst, flags::sign} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETS_H
