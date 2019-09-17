#ifndef OXTRA_CMOVZ_H
#define OXTRA_CMOVZ_H

namespace codegen {
	class Cmovz : public Cmov {
	public:
		explicit Cmovz(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVZ_H
