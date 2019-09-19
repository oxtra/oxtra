#ifndef OXTRA_SETCC_H
#define OXTRA_SETCC_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class Setcc : public codegen::Instruction {
	protected:
		explicit Setcc(const fadec::Instruction& inst, uint8_t require)
				: Instruction{inst, flags::none, require} {}

		void set_value(CodeBatch& batch, encoding::RiscVRegister reg) const;
	};
}

#include "seta.h"
#include "setbe.h"
#include "setc.h"
#include "setg.h"
#include "setge.h"
#include "setl.h"
#include "setle.h"
#include "setnc.h"
#include "setno.h"
#include "setnp.h"
#include "setns.h"
#include "setnz.h"
#include "seto.h"
#include "setp.h"
#include "sets.h"
#include "setz.h"

#endif //OXTRA_SETCC_H
