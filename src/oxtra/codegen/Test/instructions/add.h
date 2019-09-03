#ifndef OXTRA_ADD_H
#define OXTRA_ADD_H

#include "oxtra/codegen/Test/instruction.h"
#include "oxtra/codegen/Test/binary_operation.h"

namespace codegen {
	class Add : public BinaryOperation {
	public:
		explicit Add(const fadec::Instruction& inst)
			: BinaryOperation{inst, Flags::all, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		bool end_of_block() const;
	};

}
#endif //OXTRA_ADD_H
