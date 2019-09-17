#ifndef OXTRA_CMOVNS_H
#define OXTRA_CMOVNS_H

namespace codegen {
	class Cmovns : public Cmov {
	public:
		explicit Cmovns(const fadec::Instruction& inst)
				: Cmov{inst, flags::sign} {}

	private:
		// sf == 0
		void execute_operation(CodeBatch& batch) const final {
			evaluate_sign(batch, encoding::RiscVRegister::t0);
			const auto sign = batch.add(encoding::NOP());

			generate_move(batch);

			batch[sign] = encoding::BNQZ(encoding::RiscVRegister::t4,
										   (batch.size() - sign) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVNS_H
