#ifndef OXTRA_CMOVNS_H
#define OXTRA_CMOVNS_H

namespace codegen {
	class Cmovns : public Cmov {
	public:
		explicit Cmovns(const fadec::Instruction& inst)
				: Cmov{inst, flags::sign} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVNS_H
