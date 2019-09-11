#ifndef OXTRA_CEX_H
#define OXTRA_CEX_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	/**
	 * This class is for CBW, CWDE and CDQE respectively.
	 */
	class Cex : public codegen::Instruction {
	public:
		explicit Cex(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const override;
	};
}

#endif //OXTRA_CEX_H
