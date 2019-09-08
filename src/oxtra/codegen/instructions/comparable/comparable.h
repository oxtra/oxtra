#ifndef OXTRA_COMPARABLE_H
#define OXTRA_COMPARABLE_H

#include <oxtra/codegen/instruction.h>

namespace codegen {
	class Comparable : public codegen::Instruction {
	public:
		explicit Comparable(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::all, flags::none} {};

		void generate(CodeBatch& batch) const final;

		/**
		 * The implementation of the execution_operation can use registers: t3, t4, t5, t6 as needed.
		 * The content of register src must not be changed.
		 * Return the register which contains the value that should be used for updating zf, sf and pf.
		 */
		virtual encoding::RiscVRegister execute_operation(CodeBatch& batch,
														  encoding::RiscVRegister dst, encoding::RiscVRegister src) const = 0;

		/**
		 * The implementation of the execution_operation can use registers: t3, t4, t5, t6 as needed.
		 * The immediate will always be in the range [-0x800;0x7ff], allowing: "operation dest, dest, imm"
		 * Return the register which contains the value that should be used for updating zf, sf and pf.
		 */
		virtual encoding::RiscVRegister execute_operation(CodeBatch& batch,
														  encoding::RiscVRegister dst, intptr_t imm) const = 0;
	};
}

#endif //OXTRA_COMPARABLE_H
