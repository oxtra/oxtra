#ifndef OXTRA_CMOVC_H
#define OXTRA_CMOVC_H

namespace codegen {
	class Cmovc : public Cmov {
	public:
		explicit Cmovc(const fadec::Instruction& inst)
				: Cmov{inst, flags::carry} {}

	private:
		// cf == 1
		void execute_operation(CodeBatch& batch) const final {
			evaluate_carry(batch);
			const auto carry = batch.add(encoding::NOP());

			generate_move(batch);

			batch[carry] = encoding::BEQZ(encoding::RiscVRegister::t4,
										  (batch.size() - carry) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVC_H
