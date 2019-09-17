#ifndef OXTRA_CMOVNC_H
#define OXTRA_CMOVNC_H

namespace codegen {
	class Cmovnc : public Cmov {
	public:
		explicit Cmovnc(const fadec::Instruction& inst)
				: Cmov{inst, flags::carry} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVNC_H
