#ifndef OXTRA_EXECUTION_CONTEXT_H
#define OXTRA_EXECUTION_CONTEXT_H

#include <cstdint>
#include <array>
#include <spdlog/fmt/bundled/ostream.h>
#include <oxtra/codegen/Test/instruction.h>

#include "oxtra/codegen/codegen.h"

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
				uintptr_t rbp, rflags, rax, rbx, rcx, rdx, rsi, rdi, r8, r9, r10, r11, r12, r13, r14, r15;
			private:
				uintptr_t _r8, _r9;
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
						  << "\n\tsp: " << c.sp
						  << "\n\tgp: " << c.gp
						  << "\n\ttp: " << c.tp
						  << "\n\tt0: " << c.t0
						  << "\n\tt1: " << c.t1
						  << "\n\tt2: " << c.t2
						  << "\n\tfp: " << c.fp
						  << "\n\ts1: " << c.s1
						  << "\n\ta0: " << c.a0
						  << "\n\ta1: " << c.a1
						  << "\n\ta2: " << c.a2
						  << "\n\ta3: " << c.a3
						  << "\n\ta4: " << c.a4
						  << "\n\ta5: " << c.a5
						  << "\n\ta6: " << c.a6
						  << "\n\ta7: " << c.a7
						  << "\n\ts2: " << c.s2
						  << "\n\ts3: " << c.s3
						  << "\n\ts4: " << c.s4
						  << "\n\ts5: " << c.s5
						  << "\n\ts6: " << c.s6
						  << "\n\ts7: " << c.s7
						  << "\n\ts8: " << c.s8
						  << "\n\ts9: " << c.s9
						  << "\n\ts10: " << c.s10
						  << "\n\ts11: " << c.s11
						  << "\n\tt3: " << c.t3
						  << "\n\tt4: " << c.t4
						  << "\n\tt5: " << c.t5
						  << "\n\tt6: " << c.t6
						  << "\n}";
			}
		};

		/*
		 * The order and size of these attributes must not be changed!
		 * (dispatcher.s has these offsets hardcoded)
		 */

		// 0x000
		Context guest;

		// 0x0F8
		Context host;

		// 0x1F0
		codegen::CodeGenerator* codegen;

		// 0x1F8
		codegen::Instruction::FlagInfo flag_info;
	};
}

#endif //OXTRA_EXECUTION_CONTEXT_H
