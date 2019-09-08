#ifndef OXTRA_EXECUTION_CONTEXT_H
#define OXTRA_EXECUTION_CONTEXT_H

#include <cstdint>
#include <array>
#include <spdlog/fmt/bundled/ostream.h>

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
				uintptr_t rbp;
			private:
				uintptr_t _s1;
			public:
				uintptr_t rax, rbx, rcx, rdx, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15;
			private:
				uintptr_t _s8, _s9;
			public:
				uintptr_t jump_table, context;
			private:
				uintptr_t _t3, _t4, _t5, _t6;
			} map;

			std::array<uintptr_t, 31> reg;

			// spdlog
			template<class OStream>
			friend OStream& operator<<(OStream& os, const Context& c) {
				return os << "{\n\tra: " << std::hex << c.ra
						  << "\n\tsp = rsp: " << c.sp
						  << "\n\tgp: " << c.gp
						  << "\n\ttp: " << c.tp
						  << "\n\tt0: " << c.t0
						  << "\n\tt1: " << c.t1
						  << "\n\tt2: " << c.t2
						  << "\n\tfp = rbp: " << c.fp
						  << "\n\ts1: " << c.s1
						  << "\n\ta0 = rax: " << c.a0
						  << "\n\ta1 = rbx: " << c.a1
						  << "\n\ta2 = rcx: " << c.a2
						  << "\n\ta3 = rdx: " << c.a3
						  << "\n\ta4 = rsi: " << c.a4
						  << "\n\ta5 = rdi: " << c.a5
						  << "\n\ta6 = r8: " << c.a6
						  << "\n\ta7 = r9: " << c.a7
						  << "\n\ts2 = r10: " << c.s2
						  << "\n\ts3 = r11: " << c.s3
						  << "\n\ts4 = r12: " << c.s4
						  << "\n\ts5 = r13: " << c.s5
						  << "\n\ts6 = r14: " << c.s6
						  << "\n\ts7 = r15: " << c.s7
						  << "\n\ts8: " << c.s8
						  << "\n\ts9: " << c.s9
						  << "\n\ts10 = jt: " << c.s10
						  << "\n\ts11 = ctx: " << c.s11
						  << "\n\tt3: " << c.t3
						  << "\n\tt4: " << c.t4
						  << "\n\tt5: " << c.t5
						  << "\n\tt6: " << c.t6
						  << "\n}";
			}
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
	};
}

#endif //OXTRA_EXECUTION_CONTEXT_H