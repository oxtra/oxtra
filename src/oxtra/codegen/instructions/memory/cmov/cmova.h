#ifndef OXTRA_CMOVA_H
#define OXTRA_CMOVA_H

namespace codegen {
	class Cmova : public Cmov {
	public:
		explicit Cmova(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero | flags::carry} {}

	private:
		// cf == 0 && zf == 0
		void execute_operation(CodeBatch& batch) const final {
			evaluate_zero(batch);
			const auto zero = batch.add(encoding::NOP());

			evaluate_carry(batch);
			const auto carry = batch.add(encoding::NOP());

			generate_move(batch);

			batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
										 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));

			batch[carry] = encoding::BNQZ(encoding::RiscVRegister::t4,
										  (batch.size() - carry) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVA_H
