#ifndef OXTRA_SHL_H
#define OXTRA_SHL_H

#include "oxtra/codegen/instruction.h"
#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Shl : public BinaryOperation {
	public:
		explicit Shl(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::all, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		bool end_of_block() const;
	};
}

#endif //OXTRA_SHL_H
