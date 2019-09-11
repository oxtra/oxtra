#ifndef OXTRA_CSEP_H
#define OXTRA_CSEP_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	/**
	 * This class is for CWD, CDQ and CQO respectively.
	 */
	class CSep : public codegen::Instruction {
	public:
		explicit CSep(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}


#endif //OXTRA_CSEP_H
