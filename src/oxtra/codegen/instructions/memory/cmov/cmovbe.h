#ifndef OXTRA_CMOVBE_H
#define OXTRA_CMOVBE_H

namespace codegen {
	class Cmovbe : public Cmov {
	public:
		explicit Cmovbe(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero | flags::carry} {}

	private:
		// cf == 1 || zf == 1
		void execute_operation(CodeBatch& batch) const final {
			evaluate_zero(batch);
			const auto zero = batch.add(encoding::NOP());

			evaluate_carry(batch);
			const auto carry = batch.add(encoding::NOP());

			batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
										 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));

			generate_move(batch);

			batch[carry] = encoding::BEQZ(encoding::RiscVRegister::t4,
										  (batch.size() - carry) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVBE_H
