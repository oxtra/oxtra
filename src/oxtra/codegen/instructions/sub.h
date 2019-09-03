#ifndef OXTRA_SUB_H
#define OXTRA_SUB_H

#include "oxtra/codegen/instruction.h"
#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Sub : public BinaryOperation {
	public:
		explicit Sub(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		bool end_of_block() const;
	};
}

#endif //OXTRA_SUB_H
