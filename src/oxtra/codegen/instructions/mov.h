#ifndef OXTRA_MOV_H
#define OXTRA_MOV_H

#include "oxtra/codegen/instruction.h"
#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Mov : public BinaryOperation {
	public:
		explicit Mov(const fadec::Instruction& inst)
				: BinaryOperation{inst, Flags::none, Flags::none} {}

		void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		bool end_of_block() const;
	};
}

#endif //OXTRA_MOV_H
