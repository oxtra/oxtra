#ifndef OXTRA_EXECUTION_CONTEXT_H
#define OXTRA_EXECUTION_CONTEXT_H

#include <cstdint>
#include <array>

#include "oxtra/codegen/flags.h"

// forward declare CodeGenerator because we only need a pointer and to prevent a circular dependency
namespace codegen {
	class CodeGenerator;
}
namespace debugger{
	class Debugger;
}

namespace dispatcher {
	struct ExecutionContext {
		union Context {
		public:
			Context() {
				reg = {};
			}

			struct {
				uintptr_t ra, sp, gp, tp, t0, t1, t2, fp, s1, a0, a1, a2, a3, a4, a5,
						a6, a7, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, t3, t4, t5, t6;
			};

			struct {
			private:
				uintptr_t _ra;
			public:
				uintptr_t rsp;
			private:
				uintptr_t _gp, _tp, _t0, _t1, _t2;
			public:
				uintptr_t rbp, return_stack;
			public:
				uintptr_t rax, rbx, rcx, rdx, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15, call_table;
			private:
				uintptr_t _s9;
			public:
				uintptr_t jump_table, context;
			private:
				uintptr_t _t3, _t4, _t5, _t6;
			} map;

			std::array<uintptr_t, 31> reg;
		};

		/*
		 * If the order and the size of these attributes is changed,
		 * the assembly_globals file has to be updated as well.
		 */

		// 0x000
		Context guest;

		// 0x0F8
		Context host;

		// 0x1F0
		codegen::CodeGenerator* codegen;

		// 0x1F8
		debugger::Debugger* debugger;

		// 0x200
		codegen::flags::Info flag_info;

		// 0x248
		uint64_t fs_base, gs_base;

		// 0x258
		uint64_t initial_break, last_break_page, program_break;

		static constexpr uintptr_t
			fs_offset = 0x248,
			gs_offset = 0x250;
	};
}

#endif //OXTRA_EXECUTION_CONTEXT_H
