#ifndef OXTRA_CMOVGE_H
#define OXTRA_CMOVGE_H

namespace codegen {
	class Cmovge : public Cmov {
	public:
		explicit Cmovge(const fadec::Instruction& inst)
				: Cmov{inst, flags::sign | flags::overflow} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVGE_H
