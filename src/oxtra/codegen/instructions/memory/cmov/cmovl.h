#ifndef OXTRA_CMOVL_H
#define OXTRA_CMOVL_H

namespace codegen {
	class Cmovl : public Cmov {
	public:
		explicit Cmovl(const fadec::Instruction& inst)
				: Cmov{inst, flags::sign | flags::overflow} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVL_H
