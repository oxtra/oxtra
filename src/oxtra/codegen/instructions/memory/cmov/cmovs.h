#ifndef OXTRA_CMOVS_H
#define OXTRA_CMOVS_H

namespace codegen {
	class Cmovs : public Cmov {
	public:
		explicit Cmovs(const fadec::Instruction& inst)
				: Cmov{inst, flags::sign} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVS_H
