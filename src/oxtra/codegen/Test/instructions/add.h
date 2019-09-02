#ifndef OXTRA_ADD_H
#define OXTRA_ADD_H

#include "oxtra/codegen/Test/instruction.h"
#include "oxtra/codegen/Test/binary_operation.h"

namespace codegen {
	class Add : public BinaryOperation {
		explicit Add()
			: BinaryOperation{all, none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) override {
			const auto carry_reg = evalute_carry(batch);
			batch += encoding::ADD(dst, dst, carry_reg);
			batch += encoding::ADD(dst, src, dst);
		}

		static void compute_overflow();
	};

}
#endif //OXTRA_ADD_H
