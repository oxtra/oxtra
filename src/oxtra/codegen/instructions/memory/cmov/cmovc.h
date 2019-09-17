#ifndef OXTRA_CMOVC_H
#define OXTRA_CMOVC_H

namespace codegen {
	class Cmovc : public Cmov {
	public:
		explicit Cmovc(const fadec::Instruction& inst)
				: Cmov{inst, flags::carry} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVC_H
