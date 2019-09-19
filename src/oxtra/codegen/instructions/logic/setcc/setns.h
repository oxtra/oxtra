#ifndef OXTRA_SETNS_H
#define OXTRA_SETNS_H

namespace codegen {
	class Setns : public Setcc {
	public:
		explicit Setns(const fadec::Instruction& inst)
				: Setcc{inst, flags::sign} {}

		void generate(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SETNS_H
