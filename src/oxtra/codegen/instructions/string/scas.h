#ifndef OXTRA_SCAS_H
#define OXTRA_SCAS_H

#include "oxtra/codegen/instructions/string/repeatable.h"

namespace codegen {
	class Scas : public RepeatableFlag {
	public:
		explicit Scas(const fadec::Instruction& inst)
				: RepeatableFlag{inst} {}

		void generate(CodeBatch& batch) const final;

	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_SCAS_H
