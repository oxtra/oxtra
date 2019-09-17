#ifndef OXTRA_CMOVBE_H
#define OXTRA_CMOVBE_H

namespace codegen {
	class Cmovbe : public Cmov {
	public:
		explicit Cmovbe(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero | flags::carry} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVBE_H
