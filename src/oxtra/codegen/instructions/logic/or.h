#ifndef OXTRA_OR_H
#define OXTRA_OR_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Or : public BinaryOperation {
	public:
		explicit Or(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;
	};
}


#endif //OXTRA_OR_H
