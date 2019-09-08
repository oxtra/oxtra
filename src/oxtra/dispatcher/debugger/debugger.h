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
			static constexpr uint16_t none 			= 0x0000u;
			static constexpr uint16_t init 			= 0x0001u;
			static constexpr uint16_t await_sblock 	= 0x0002u;
			static constexpr uint16_t await_eblock 	= 0x0004u;
			static constexpr uint16_t await_counter = 0x0008u;
			static constexpr uint16_t await_step	= 0x0010u;
			static constexpr uint16_t await 		= 0x001eu;
			static constexpr uint16_t print_reg 	= 0x0100u;
			static constexpr uint16_t reg_riscv 	= 0x0200u;
			static constexpr uint16_t reg_dec 		= 0x0400u;
			static constexpr uint16_t print_x86 	= 0x0800u;
			static constexpr uint16_t print_riscv 	= 0x1000u;
			static constexpr uint16_t print_flags	= 0x2000u;
		};

		struct BlockEntry {
			codegen::codestore::BlockEntry* entry;
			utils::host_addr_t riscv_end;

			BlockEntry(codegen::codestore::BlockEntry* ent, utils::host_addr_t end) : entry(ent), riscv_end(end) {}
		};

	private:
		static constexpr uintptr_t halt_break = 0x0400;
		static Debugger* active_debugger;
	private:
		uint8_t _halt;
		uint8_t _bp_count;
		uintptr_t _bp_array[256]{};
		uintptr_t _bp_x86_array[256]{};
		uint32_t _state;
		uint64_t _bp_counter;
		BlockEntry* _current;
		std::vector<BlockEntry> _blocks;

	public:
		Debugger();

		~Debugger();

		static void begin_block(codegen::CodeBatch& batch);

		static void insert(codegen::CodeBatch& batch, codegen::Instruction* inst);

		static void end_block(codegen::CodeBatch& batch, codegen::codestore::BlockEntry* block);

	private:
		void entry(dispatcher::ExecutionContext* context, uintptr_t break_point);

		utils::guest_addr_t resolve_block(utils::host_addr_t address);

		utils::guest_addr_t enter_break(uintptr_t break_point, utils::host_addr_t address);

		void update_break_points(BlockEntry& block);

		std::string parse_input();

		std::string print_number(uint64_t nbr, bool hex);

		std::string print_reg(dispatcher::ExecutionContext* context);

		std::string print_assembly(utils::guest_addr_t guest, utils::host_addr_t host);

		std::string print_flags(dispatcher::ExecutionContext* context);
	};
}

#endif //OXTRA_DEBUGGER_H
