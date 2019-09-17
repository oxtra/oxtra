#ifndef OXTRA_CMOVNP_H
#define OXTRA_CMOVNP_H

namespace codegen {
	class Cmovnp : public Cmov {
	public:
		explicit Cmovnp(const fadec::Instruction& inst)
				: Cmov{inst, flags::parity} {}

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMOVNP_H
