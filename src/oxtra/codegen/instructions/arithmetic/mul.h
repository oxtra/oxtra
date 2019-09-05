#ifndef OXTRA_MUL_H
#define OXTRA_MUL_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Mul : public codegen::Instruction {
	public:
		explicit Mul(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::all, Flags::none} {}

		void generate(CodeBatch& batch) const override;

	private:
		/**
		 * This method loads an operand lazily, which means that if the operand is a 64bit register, it will not be copied into a temp register.
		 * The returned register contains the register that can be used (which will either be the specified register or the operand register itself).
		 */
		encoding::RiscVRegister translate_operand_lazy(codegen::CodeBatch& batch, size_t index,
													   encoding::RiscVRegister reg, encoding::RiscVRegister temp_a,
													   encoding::RiscVRegister temp_b) const;
	};
}

#endif //OXTRA_MUL_H
