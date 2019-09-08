#ifndef OXTRA_DEBUGGER_H
#define OXTRA_DEBUGGER_H

#include "oxtra/codegen/code_batch.h"
#include "oxtra/dispatcher/execution_context.h"

namespace debugger {
	class DebuggerBatch : public codegen::CodeBatchImpl {

		virtual size_t add(utils::riscv_instruction_t inst) override;

		virtual void end() override;

		virtual void print() const override;

		virtual void reset() override;
	};

	class Debugger {
	private:
		static void debug_entry(dispatcher::ExecutionContext* context);

	};
}

#endif //OXTRA_DEBUGGER_H
