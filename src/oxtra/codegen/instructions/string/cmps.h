#ifndef OXTRA_CMPS_H
#define OXTRA_CMPS_H

#include "oxtra/codegen/instructions/string/repeatable.h"

namespace codegen {
	class Cmps : public RepeatableFlag {
	public:
		explicit Cmps(const fadec::Instruction& inst)
				: RepeatableFlag{inst} {}
	private:
		void execute_operation(CodeBatch& batch) const final;
	};
}

#endif //OXTRA_CMPS_H
