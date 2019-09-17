#ifndef OXTRA_CMOVNO_H
#define OXTRA_CMOVNO_H

namespace codegen {
	class Cmovno : public Cmov {
	public:
		explicit Cmovno(const fadec::Instruction& inst)
				: Cmov{inst, flags::overflow} {}

	private:
		// of == 0
		void execute_operation(CodeBatch& batch) const final {
			evaluate_overflow(batch);
			const auto overflow = batch.add(encoding::NOP());

			generate_move(batch);

			batch[overflow] = encoding::BNQZ(encoding::RiscVRegister::t4,
										  (batch.size() - overflow) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVNO_H
