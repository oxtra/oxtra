#ifndef OXTRA_CMOVLE_H
#define OXTRA_CMOVLE_H

namespace codegen {
	class Cmovle : public Cmov {
	public:
		explicit Cmovle(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero | flags::sign | flags::overflow} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVLE_H
