#ifndef OXTRA_CMOVZ_H
#define OXTRA_CMOVZ_H

namespace codegen {
	class Cmovz : public Cmov {
	public:
		explicit Cmovz(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero} {}

	private:
		// zf == 1
		void execute_operation(CodeBatch& batch) const final {
			evaluate_zero(batch);
			const auto zero = batch.add(encoding::NOP());

			generate_move(batch);

			batch[zero] = encoding::BEQZ(encoding::RiscVRegister::t4,
										  (batch.size() - zero) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVZ_H
