#ifndef OXTRA_CMOVNP_H
#define OXTRA_CMOVNP_H

namespace codegen {
	class Cmovnp : public Cmov {
	public:
		explicit Cmovnp(const fadec::Instruction& inst)
				: Cmov{inst, flags::parity} {}

	private:
		// pf == 0
		void execute_operation(CodeBatch& batch) const final {
			evaluate_parity(batch, encoding::RiscVRegister::t0);
			const auto parity = batch.add(encoding::NOP());

			generate_move(batch);

			batch[parity] = encoding::BNQZ(encoding::RiscVRegister::t4,
											 (batch.size() - parity) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVNP_H
