#ifndef OXTRA_CMOVO_H
#define OXTRA_CMOVO_H

namespace codegen {
	class Cmovo : public Cmov {
	public:
		explicit Cmovo(const fadec::Instruction& inst)
				: Cmov{inst, flags::overflow} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVO_H
