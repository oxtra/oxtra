#ifndef OXTRA_CONTEXT_H
#define OXTRA_CONTEXT_H

#include <cstdlib>

#define capture_context(reg) asm( \
"sd ra, 0x00("#reg");" \
"sd sp, 0x08("#reg");" \
"sd gp, 0x10("#reg");" \
"sd tp, 0x18("#reg");" \
"sd t0, 0x20("#reg");" \
"sd t1, 0x28("#reg");" \
"sd t2, 0x30("#reg");" \
"sd fp, 0x38("#reg");" \
"sd s1, 0x40("#reg");" \
"sd a0, 0x48("#reg");" \
"sd a1, 0x50("#reg");" \
"sd a2, 0x58("#reg");" \
"sd a3, 0x60("#reg");" \
"sd a4, 0x68("#reg");" \
"sd a5, 0x70("#reg");" \
"sd a6, 0x78("#reg");" \
"sd a7, 0x80("#reg");" \
"sd s2, 0x88("#reg");" \
"sd s3, 0x90("#reg");" \
"sd s4, 0x98("#reg");" \
"sd s5, 0xA0("#reg");" \
"sd s6, 0xA8("#reg");" \
"sd s7, 0xB0("#reg");" \
"sd s8, 0xB8("#reg");" \
"sd s9, 0xC0("#reg");" \
"sd s10, 0xC8("#reg");" \
"sd s11, 0xD0("#reg");" \
"sd t3, 0xD8("#reg");" \
"sd t4, 0xE0("#reg");" \
"sd t5, 0xE8("#reg");" \
"sd t6, 0xF0("#reg");" \
)

#define restore_context(reg) asm( \
"ld ra, 0x00("#reg");" \
"ld sp, 0x08("#reg");" \
"ld gp, 0x10("#reg");" \
"ld tp, 0x18("#reg");" \
"ld t0, 0x20("#reg");" \
"ld t1, 0x28("#reg");" \
"ld t2, 0x30("#reg");" \
"ld fp, 0x38("#reg");" \
"ld s1, 0x40("#reg");" \
"ld a0, 0x48("#reg");" \
"ld a1, 0x50("#reg");" \
"ld a2, 0x58("#reg");" \
"ld a3, 0x60("#reg");" \
"ld a4, 0x68("#reg");" \
"ld a5, 0x70("#reg");" \
"ld a6, 0x78("#reg");" \
"ld a7, 0x80("#reg");" \
"ld s2, 0x88("#reg");" \
"ld s3, 0x90("#reg");" \
"ld s4, 0x98("#reg");" \
"ld s5, 0xA0("#reg");" \
"ld s6, 0xA8("#reg");" \
"ld s7, 0xB0("#reg");" \
"ld s8, 0xB8("#reg");" \
"ld s9, 0xC0("#reg");" \
"ld s10, 0xC8("#reg");" \
"ld s11, 0xD0("#reg");" \
"ld t3, 0xD8("#reg");" \
"ld t4, 0xE0("#reg");" \
"ld t5, 0xE8("#reg");" \
"ld t6, 0xF0("#reg");" \
)

namespace dispatcher {
	class Context {
	public:
		size_t
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
			return os << "{\n\tra: " << c.ra
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
