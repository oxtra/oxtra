#ifndef OXTRA_CONTEXT_H
#define OXTRA_CONTEXT_H

#include <cstdint>
#include <spdlog/fmt/bundled/ostream.h>
namespace dispatcher {
	class Context {
	public:
		uintptr_t
			ra = 0,
			sp = 0,
			gp = 0,
			tp = 0,
			t0 = 0,
			t1 = 0,
			t2 = 0,
			fp = 0,
			s1 = 0,
			a0 = 0,
			a1 = 0,
			a2 = 0,
			a3 = 0,
			a4 = 0,
			a5 = 0,
			a6 = 0,
			a7 = 0,
			s2 = 0,
			s3 = 0,
			s4 = 0,
			s5 = 0,
			s6 = 0,
			s7 = 0,
			s8 = 0,
			s9 = 0,
			s10 = 0,
			s11 = 0,
			t3 = 0,
			t4 = 0,
			t5 = 0,
			t6 = 0;

		// spdlog
		template <class OStream>
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
}

#endif //OXTRA_CONTEXT_H
