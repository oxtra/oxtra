#ifndef OXTRA_TEST_H
#define OXTRA_TEST_H

#include "oxtra/codegen/instructions/comparable/comparable.h"

namespace codegen {
	class Test : public Comparable {
	public:
		explicit Test(const fadec::Instruction& inst)
				: Comparable{inst} {}

		encoding::RiscVRegister execute_operation(CodeBatch& batch,
												  encoding::RiscVRegister dst, encoding::RiscVRegister src) const override;

		encoding::RiscVRegister execute_operation(CodeBatch& batch,
												  encoding::RiscVRegister dst, intptr_t imm) const override;
	};
}

#endif //OXTRA_TEST_H
