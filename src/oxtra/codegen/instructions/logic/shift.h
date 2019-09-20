#ifndef OXTRA_SHIFT_H
#define OXTRA_SHIFT_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class ShiftCl : public codegen::Instruction {
	public:
		explicit ShiftCl(const fadec::Instruction& inst)
			: Instruction{inst, flags::all, flags::none} {}

		void generate(CodeBatch& batch) const final;

	protected:
		virtual void execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const = 0;
	};
}

#include "sar.h"
#include "shl.h"
#include "shr.h"

#endif //OXTRA_SHIFT_H
