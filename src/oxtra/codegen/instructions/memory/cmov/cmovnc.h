#ifndef OXTRA_CMOVNC_H
#define OXTRA_CMOVNC_H

namespace codegen {
	class Cmovnc : public Cmov {
	public:
		explicit Cmovnc(const fadec::Instruction& inst)
				: Cmov{inst, flags::carry} {}

	private:
		// cf == 0
		void execute_operation(CodeBatch& batch) const final {
			evaluate_carry(batch);
			const auto carry = batch.add(encoding::NOP());

			generate_move(batch);

			batch[carry] = encoding::BNQZ(encoding::RiscVRegister::t4,
										  (batch.size() - carry) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVNC_H
