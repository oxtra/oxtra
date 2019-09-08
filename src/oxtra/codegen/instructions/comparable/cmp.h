#ifndef OXTRA_CMP_H
#define OXTRA_CMP_H

#include "oxtra/codegen/instructions/comparable/comparable.h"

namespace codegen {
	class Cmp : public Comparable {
	public:
		explicit Cmp(const fadec::Instruction& inst)
				: Comparable{inst} {}

		encoding::RiscVRegister execute_operation(CodeBatch& batch,
												  encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		encoding::RiscVRegister execute_operation(CodeBatch& batch,
												  encoding::RiscVRegister dst, intptr_t imm) const override;
	};
}

#endif //OXTRA_CMP_H
