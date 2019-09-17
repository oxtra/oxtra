#ifndef OXTRA_CMOVNZ_H
#define OXTRA_CMOVNZ_H

namespace codegen {
	class Cmovnz : public Cmov {
	public:
		explicit Cmovnz(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVNZ_H
