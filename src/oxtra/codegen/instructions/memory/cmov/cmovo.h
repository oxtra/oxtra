#ifndef OXTRA_CMOVO_H
#define OXTRA_CMOVO_H

namespace codegen {
	class Cmovo : public Cmov {
	public:
		explicit Cmovo(const fadec::Instruction& inst)
				: Cmov{inst, flags::overflow} {}

	private:
		// of == 1
		void execute_operation(CodeBatch& batch) const final {
			evaluate_overflow(batch);
			const auto overflow = batch.add(encoding::NOP());

			generate_move(batch);

			batch[overflow] = encoding::BEQZ(encoding::RiscVRegister::t4,
											 (batch.size() - overflow) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVO_H
