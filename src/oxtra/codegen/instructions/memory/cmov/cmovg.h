#ifndef OXTRA_CMOVG_H
#define OXTRA_CMOVG_H

namespace codegen {
	class Cmovg : public Cmov {
	public:
		explicit Cmovg(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero | flags::sign | flags::overflow} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVG_H
