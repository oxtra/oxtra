#ifndef OXTRA_DEBUGGER_H
#define OXTRA_DEBUGGER_H

#include "oxtra/codegen/instruction.h"
#include "oxtra/dispatcher/execution_context.h"
#include "oxtra/codegen/codestore/codestore.h"

namespace debugger {
	class DebuggerBatch : public codegen::CodeBatchImpl {
		virtual void print() const override;
	};

	class Debugger {
	private:
		static constexpr uintptr_t halt_break = 0x0400;
		static Debugger* active_debugger;
	private:
		uint8_t halt;
		uint8_t bp_count;
		uintptr_t bp_array[256]{};

	public:
		Debugger();

		~Debugger();

		static void begin_block(codegen::CodeBatch& batch);

		static void insert(codegen::CodeBatch& batch, codegen::Instruction* inst);

		static void end_block(codegen::CodeBatch& batch, codegen::codestore::BlockEntry* block);

	private:
		void entry(dispatcher::ExecutionContext* context, uintptr_t break_point);
	};
}

#endif //OXTRA_DEBUGGER_H
