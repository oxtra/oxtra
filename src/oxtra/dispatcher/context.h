#ifndef OXTRA_CONTEXT_H
#define OXTRA_CONTEXT_H

#include <cstdint>
#include <spdlog/fmt/bundled/ostream.h>

#define capture_context_s11 asm( \
"sd ra, 0x00(s11);" \
"sd sp, 0x08(s11);" \
"sd gp, 0x10(s11);" \
"sd tp, 0x18(s11);" \
"sd t0, 0x20(s11);" \
"sd t1, 0x28(s11);" \
"sd t2, 0x30(s11);" \
"sd fp, 0x38(s11);" \
"sd s1, 0x40(s11);" \
"sd a0, 0x48(s11);" \
"sd a1, 0x50(s11);" \
"sd a2, 0x58(s11);" \
"sd a3, 0x60(s11);" \
"sd a4, 0x68(s11);" \
"sd a5, 0x70(s11);" \
"sd a6, 0x78(s11);" \
"sd a7, 0x80(s11);" \
"sd s2, 0x88(s11);" \
"sd s3, 0x90(s11);" \
"sd s4, 0x98(s11);" \
"sd s5, 0xA0(s11);" \
"sd s6, 0xA8(s11);" \
"sd s7, 0xB0(s11);" \
"sd s8, 0xB8(s11);" \
"sd s9, 0xC0(s11);" \
"sd s10, 0xC8(s11);" \
"sd t3, 0xD8(s11);" \
"sd t4, 0xE0(s11);" \
"sd t5, 0xE8(s11);" \
"sd t6, 0xF0(s11);" \
"sd s11, 0xD0(s11);" \
);

#define restore_context_s11 asm( \
"ld ra, 0x00(s11);" \
"ld sp, 0x08(s11);" \
"ld gp, 0x10(s11);" \
"ld tp, 0x18(s11);" \
"ld t0, 0x20(s11);" \
"ld t1, 0x28(s11);" \
"ld t2, 0x30(s11);" \
"ld fp, 0x38(s11);" \
"ld s1, 0x40(s11);" \
"ld a0, 0x48(s11);" \
"ld a1, 0x50(s11);" \
"ld a2, 0x58(s11);" \
"ld a3, 0x60(s11);" \
"ld a4, 0x68(s11);" \
"ld a5, 0x70(s11);" \
"ld a6, 0x78(s11);" \
"ld a7, 0x80(s11);" \
"ld s2, 0x88(s11);" \
"ld s3, 0x90(s11);" \
"ld s4, 0x98(s11);" \
"ld s5, 0xA0(s11);" \
"ld s6, 0xA8(s11);" \
"ld s7, 0xB0(s11);" \
"ld s8, 0xB8(s11);" \
"ld s9, 0xC0(s11);" \
"ld s10, 0xC8(s11);" \
"ld t3, 0xD8(s11);" \
"ld t4, 0xE0(s11);" \
"ld t5, 0xE8(s11);" \
"ld t6, 0xF0(s11);" \
"ld s11, 0xD0(s11);" \
);

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
