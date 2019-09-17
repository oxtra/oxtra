#ifndef OXTRA_CMOVG_H
#define OXTRA_CMOVG_H

namespace codegen {
	class Cmovg : public Cmov {
	public:
		explicit Cmovg(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero | flags::sign | flags::overflow} {}

	private:
		// zf == 0 && sf == of
		void execute_operation(CodeBatch& batch) const final {
			evaluate_zero(batch);
			const auto zero = batch.add(encoding::NOP());

			evaluate_sign(batch, encoding::RiscVRegister::t0);
			batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

			evaluate_overflow(batch);
			const auto equal = batch.add(encoding::NOP());

			generate_move(batch);

			batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
										 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));

			batch[equal] = encoding::BNE(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4,
										  (batch.size() - equal) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVG_H
