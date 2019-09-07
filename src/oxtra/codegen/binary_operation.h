#ifndef OXTRA_BINARY_OPERATION_H
#define OXTRA_BINARY_OPERATION_H

#include "instruction.h"

namespace codegen {
	class BinaryOperation : public codegen::Instruction {
	private:
		bool dest_value;
	protected:
		explicit BinaryOperation(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool dest_value);

		void generate(CodeBatch& batch) const final;

		/**
		 * The implementation of the execution_operation can use registers: t3, t4, t5, t6 as needed.
		 * The content of register src must not be changed.
		 */
		virtual void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const = 0;

		/**
		 * The implementation of the execution_operation can use registers: t3, t4, t5, t6 as needed.
		 * The immediate will always be in the range [-0x800;0x7ff], allowing: "operation dest, dest, imm"
		 */
		virtual void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const;
	};
}

#endif //OXTRA_BINARY_OPERATION_H
