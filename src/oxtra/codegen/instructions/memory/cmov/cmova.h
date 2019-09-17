#ifndef OXTRA_CMOVA_H
#define OXTRA_CMOVA_H

namespace codegen {
	class Cmova : public Cmov {
	public:
		explicit Cmova(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero | flags::carry} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVA_H
