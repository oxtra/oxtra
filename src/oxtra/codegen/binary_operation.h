#ifndef OXTRA_BINARY_OPERATION_H
#define OXTRA_BINARY_OPERATION_H

#include "instruction.h"

namespace codegen {
	class BinaryOperation : public codegen::Instruction {
	protected:
		explicit BinaryOperation(const fadec::Instruction& inst, uint8_t update, uint8_t require);

		void generate(CodeBatch& batch) const final;

		virtual void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const = 0;
	};
}

#endif //OXTRA_BINARY_OPERATION_H
