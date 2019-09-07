#ifndef OXTRA_DEBUGGER_H
#define OXTRA_DEBUGGER_H

#include "oxtra/codegen/code_batch.h"

namespace debugger {
	class DebuggerBatch : public codegen::CodeBatchImpl {

		virtual size_t add(utils::riscv_instruction_t inst) override;

		virtual void end() override;

		virtual void print() const override;
	};

	class Debugger {

	};
}

#endif //OXTRA_DEBUGGER_H
