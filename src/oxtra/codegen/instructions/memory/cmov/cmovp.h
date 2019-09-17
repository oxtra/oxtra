#ifndef OXTRA_CMOVP_H
#define OXTRA_CMOVP_H

namespace codegen {
	class Cmovp : public Cmov {
	public:
		explicit Cmovp(const fadec::Instruction& inst)
				: Cmov{inst, flags::parity} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVP_H
