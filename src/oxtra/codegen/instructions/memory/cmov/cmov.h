#ifndef OXTRA_CMOV_H
#define OXTRA_CMOV_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Cmov : public codegen::Instruction {
	public:
		explicit Cmov(const fadec::Instruction& inst, uint8_t require)
			: codegen::Instruction{inst, flags::none, require} {}

		void generate(CodeBatch& batch) const final;
	protected:
		virtual void execute_operation(CodeBatch& batch) const = 0;

		/**
		 * This should be called in every execute operation if the condition is true.
		 */
		void generate_move(CodeBatch& batch) const;
	};
}

#include "cmova.h"
#include "cmovbe.h"
#include "cmovc.h"
#include "cmovg.h"
#include "cmovge.h"
#include "cmovl.h"
#include "cmovle.h"
#include "cmovnc.h"
#include "cmovno.h"
#include "cmovnp.h"
#include "cmovns.h"
#include "cmovnz.h"
#include "cmovo.h"
#include "cmovp.h"
#include "cmovs.h"
#include "cmovz.h"

#endif //OXTRA_CMOV_H
