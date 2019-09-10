#ifndef OXTRA_MOVS_H
#define OXTRA_MOVS_H

#include "oxtra/codegen/instructions/string/repeatable.h"

namespace codegen {
	class Movs : public Repeatable {
	public:
		explicit Movs(const fadec::Instruction& inst)
				: Repeatable{inst} {}

		void execute_operation(CodeBatch& batch) const final;
	};
}
#endif //OXTRA_MOVS_H
