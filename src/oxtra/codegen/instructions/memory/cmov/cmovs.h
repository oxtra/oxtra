#ifndef OXTRA_CMOVS_H
#define OXTRA_CMOVS_H

namespace codegen {
	class Cmovs : public Cmov {
	public:
		explicit Cmovs(const fadec::Instruction& inst)
				: Cmov{inst, flags::sign} {}

	private:
		// sf == 1
		void execute_operation(CodeBatch& batch) const final {
			evaluate_sign(batch, encoding::RiscVRegister::t0);
			const auto sign = batch.add(encoding::NOP());

			generate_move(batch);

			batch[sign] = encoding::BEQZ(encoding::RiscVRegister::t4,
										 (batch.size() - sign) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVS_H
