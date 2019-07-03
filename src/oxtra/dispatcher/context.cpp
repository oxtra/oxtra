#include "context.h"

using namespace dispatcher;

void __attribute__((always_inline)) dispatcher::Context::capture(Context* a0) {

	/*
	 * this is a temporary function for testing.
	 * a pointer to the context will already be stored in s11 (or it will be recoverable by just adding a static offset),
	 * when we capture the context on a host_enter
	 */

	asm("mv s11, a0;"
		"sd ra, 0x00(s11);"
		"sd sp, 0x08(s11);"
		"sd gp, 0x10(s11);"
		"sd tp, 0x18(s11);"
		"sd t0, 0x20(s11);"
		"sd t1, 0x28(s11);"
		"sd t2, 0x30(s11);"
		"sd fp, 0x38(s11);"
		"sd s1, 0x40(s11);"
		"sd a0, 0x48(s11);"
		"sd a1, 0x50(s11);"
		"sd a2, 0x58(s11);"
		"sd a3, 0x60(s11);"
		"sd a4, 0x68(s11);"
		"sd a5, 0x70(s11);"
		"sd a6, 0x78(s11);"
		"sd a7, 0x80(s11);"
		"sd s2, 0x88(s11);"
		"sd s3, 0x90(s11);"
		"sd s4, 0x98(s11);"
		"sd s5, 0xA0(s11);"
		"sd s6, 0xA8(s11);"
		"sd s7, 0xB0(s11);"
		"sd s8, 0xB8(s11);"
		"sd s9, 0xC0(s11);"
		"sd s10, 0xC8(s11);"
		"sd s11, 0xD0(s11);"
		"sd t3, 0xD8(s11);"
		"sd t4, 0xE0(s11);"
		"sd t5, 0xE8(s11);"
		"sd t6, 0xF0(s11);"
	);
}

void __attribute__((always_inline)) Context::restore(dispatcher::Context* a0) {
	asm("mv s11, a0;"
		"ld ra, 0x00(s11);"
		"ld sp, 0x08(s11);"
		"ld gp, 0x10(s11);"
		"ld tp, 0x18(s11);"
		"ld t0, 0x20(s11);"
		"ld t1, 0x28(s11);"
		"ld t2, 0x30(s11);"
		"ld fp, 0x38(s11);"
		"ld s1, 0x40(s11);"
		"ld a0, 0x48(s11);"
		"ld a1, 0x50(s11);"
		"ld a2, 0x58(s11);"
		"ld a3, 0x60(s11);"
		"ld a4, 0x68(s11);"
		"ld a5, 0x70(s11);"
		"ld a6, 0x78(s11);"
		"ld a7, 0x80(s11);"
		"ld s2, 0x88(s11);"
		"ld s3, 0x90(s11);"
		"ld s4, 0x98(s11);"
		"ld s5, 0xA0(s11);"
		"ld s6, 0xA8(s11);"
		"ld s7, 0xB0(s11);"
		"ld s8, 0xB8(s11);"
		"ld s9, 0xC0(s11);"
		"ld s10, 0xC8(s11);"
		"ld s11, 0xD0(s11);"
		"ld t3, 0xD8(s11);"
		"ld t4, 0xE0(s11);"
		"ld t5, 0xE8(s11);"
		"ld t6, 0xF0(s11);"
	);
}