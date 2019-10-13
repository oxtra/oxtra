#ifndef OXTRA_JCC_H
#define OXTRA_JCC_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Jcc : public codegen::Instruction {
	protected:
		explicit Jcc(const fadec::Instruction& inst, uint8_t required)
				: codegen::Instruction{inst, flags::none, required} {}

		/**
		 * Generates the instructions that perform the jump.
		 * This should be called if the condition is true.
		 */
		void generate_jump(CodeBatch& batch) const;

		uintptr_t branch_address() const final;

		uint8_t control_flow_dimension() const final;
	};
}

#include "ja.h"
#include "jbe.h"
#include "jc.h"
#include "jcxz.h"
#include "jg.h"
#include "jge.h"
#include "jl.h"
#include "jle.h"
#include "jnc.h"
#include "jno.h"
#include "jnp.h"
#include "jns.h"
#include "jnz.h"
#include "jo.h"
#include "jp.h"
#include "js.h"
#include "jz.h"

#endif //OXTRA_JCC_H
