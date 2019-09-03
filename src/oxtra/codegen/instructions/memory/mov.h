#ifndef OXTRA_MOV_H
#define OXTRA_MOV_H

#include "oxtra/codegen/binary_operation.h"

namespace codegen {
	class Mov : public codegen::Instruction {
	public:
		explicit Mov(const fadec::Instruction& inst)
				: codegen::Instruction{inst, Flags::none, Flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_MOV_H
