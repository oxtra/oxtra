#ifndef OXTRA_CMOVP_H
#define OXTRA_CMOVP_H

namespace codegen {
	class Cmovp : public Cmov {
	public:
		explicit Cmovp(const fadec::Instruction& inst)
				: Cmov{inst, flags::parity} {}

	private:
		// pf == 1
		void execute_operation(CodeBatch& batch) const final {
			evaluate_parity(batch, encoding::RiscVRegister::t0);
			const auto parity = batch.add(encoding::NOP());

			generate_move(batch);

			batch[parity] = encoding::BEQZ(encoding::RiscVRegister::t4,
										   (batch.size() - parity) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVP_H
