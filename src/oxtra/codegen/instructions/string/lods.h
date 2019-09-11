#ifndef OXTRA_LODS_H
#define OXTRA_LODS_H

#include "oxtra/codegen/instructions/string/repeatable.h"

namespace codegen {
	class Lods : public Repeatable {
	public:
		explicit Lods(const fadec::Instruction& inst)
				: Repeatable{inst} {}

		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_LODS_H
