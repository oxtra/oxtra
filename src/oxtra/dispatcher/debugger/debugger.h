#ifndef OXTRA_DEBUGGER_H
#define OXTRA_DEBUGGER_H

#include "oxtra/codegen/code_batch.h"
#include "oxtra/dispatcher/execution_context.h"

namespace debugger {
	class DebuggerBatch : public codegen::CodeBatchImpl {

		virtual size_t add(utils::riscv_instruction_t inst) override;

		virtual void begin(const fadec::Instruction* inst, bool eob, uint8_t update, uint8_t require) override;

		virtual void end() override;

		virtual void print() const override;
	};

	class Debugger {
	private:
		static constexpr uintptr_t halt_break = 0x0400;
	private:
		uint8_t halt;
		uint8_t bp_count;
		uintptr_t bp_array[256];

	public:
		Debugger();

	private:
		void entry(dispatcher::ExecutionContext* context, uintptr_t break_point);
	};
}

#endif //OXTRA_DEBUGGER_H
