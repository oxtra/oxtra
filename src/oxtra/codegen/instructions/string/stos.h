#ifndef OXTRA_STOS_H
#define OXTRA_STOS_H

#include "oxtra/codegen/instructions/string/repeatable.h"

namespace codegen {
	class Stos : public Repeatable {
	public:
		explicit Stos(const fadec::Instruction& inst)
				: Repeatable{inst} {}

		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_STOS_H
