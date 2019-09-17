#ifndef OXTRA_CMOVNZ_H
#define OXTRA_CMOVNZ_H

namespace codegen {
	class Cmovnz : public Cmov {
	public:
		explicit Cmovnz(const fadec::Instruction& inst)
				: Cmov{inst, flags::zero} {}

	private:
		// zf == 0
		void execute_operation(CodeBatch& batch) const final {
			evaluate_zero(batch);
			const auto zero = batch.add(encoding::NOP());

			generate_move(batch);

			batch[zero] = encoding::BNQZ(encoding::RiscVRegister::t4,
										 (batch.size() - zero) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVNZ_H
