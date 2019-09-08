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
		struct DebugState {
			static constexpr uint32_t none 		= 0;
			static constexpr uint32_t reg_print = 0;
			static constexpr uint32_t reg_riscv = 0;
			static constexpr uint32_t reg_dec 	= 0;
			static constexpr uint32_t init		= 0;
		};

	private:
		static constexpr uintptr_t halt_break = 0x0400;
		static Debugger* active_debugger;
	private:
		uint8_t halt;
		uint8_t bp_count;
		uintptr_t bp_array[256]{};
		uintptr_t bp_x86_array[256]{};
		uint32_t state;
		uint64_t debug_counter;
		std::vector<codegen::codestore::BlockEntry*> blocks;

	public:
		Debugger();

		~Debugger();

		static void begin_block(codegen::CodeBatch& batch);

		static void insert(codegen::CodeBatch& batch, codegen::Instruction* inst);

		static void end_block(codegen::CodeBatch& batch, codegen::codestore::BlockEntry* block);

	private:
		void entry(dispatcher::ExecutionContext* context, uintptr_t break_point);

		bool parse_input(std::string& input);

		std::string print_reg(dispatcher::ExecutionContext* context);

		std::string print_number(uint64_t nbr, bool hex);
	};
}

#endif //OXTRA_DEBUGGER_H
