#ifndef OXTRA_CMOVL_H
#define OXTRA_CMOVL_H

namespace codegen {
	class Cmovl : public Cmov {
	public:
		explicit Cmovl(const fadec::Instruction& inst)
				: Cmov{inst, flags::sign | flags::overflow} {}

	private:
		// sf != of
		void execute_operation(CodeBatch& batch) const final {
			evaluate_sign(batch, encoding::RiscVRegister::t0);
			batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

			evaluate_overflow(batch);
			const auto equal = batch.add(encoding::NOP());

			generate_move(batch);

			batch[equal] = encoding::BEQ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4,
										 (batch.size() - equal) * sizeof(utils::riscv_instruction_t));
		}
	};
}

#endif //OXTRA_CMOVL_H
