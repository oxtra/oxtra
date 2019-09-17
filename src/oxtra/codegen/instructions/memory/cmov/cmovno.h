#ifndef OXTRA_CMOVNO_H
#define OXTRA_CMOVNO_H

namespace codegen {
	class Cmovno : public Cmov {
	public:
		explicit Cmovno(const fadec::Instruction& inst)
				: Cmov{inst, flags::overflow} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVNO_H
