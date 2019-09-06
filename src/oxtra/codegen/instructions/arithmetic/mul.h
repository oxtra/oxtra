#ifndef OXTRA_MUL_H
#define OXTRA_MUL_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Mul : public codegen::Instruction {
	public:
		explicit Mul(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::carry | Flags::overflow, Flags::none} {}

		void generate(CodeBatch& batch) const override;

	private:
		/**
		 * Contrary to the normal translate_operand method, this method can sign extend and ensures
		 * that the destination register will only have the correct bits set. Further, the destination register itself will be
		 * returned instead of the memory address. This can be used so that 64 bit registers do not have to be moved.
		 * @param batch
		 * @param index
		 * @param reg
		 * @param temp_a
		 * @param temp_b
		 * @param sign_extend
		 * @return
		 */
		encoding::RiscVRegister load_operand(codegen::CodeBatch& batch, size_t index,
											 encoding::RiscVRegister reg, encoding::RiscVRegister temp_a,
											 encoding::RiscVRegister temp_b, bool sign_extend) const;
	};
}

#endif //OXTRA_MUL_H
