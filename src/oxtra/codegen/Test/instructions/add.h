#ifndef OXTRA_ADD_H
#define OXTRA_ADD_H

#include "oxtra/codegen/Test/instruction.h"
#include "oxtra/codegen/Test/binary_operation.h"

namespace codegen {
	class Add : public BinaryOperation {
	public:
		explicit Add(const fadec::Instruction& inst)
			: BinaryOperation{inst, all, none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override {
			batch += encoding::ADD(dst, src, dst);
		}

		bool end_of_block() const {
			return false;
		}
	};

}
#endif //OXTRA_ADD_H
