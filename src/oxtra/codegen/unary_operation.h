#ifndef OXTRA_UNARY_OPERATION_H
#define OXTRA_UNARY_OPERATION_H

#include "instruction.h"

namespace codegen {
	class UnaryOperation : public codegen::Instruction {
	public:
		explicit UnaryOperation(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool eob = false);

		void generate(CodeBatch& batch) const final;

		virtual void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst) const = 0;
	};
}

#endif //OXTRA_UNARY_OPERATION_H
