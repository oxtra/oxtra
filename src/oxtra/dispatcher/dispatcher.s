.section .text
.global _ZN10dispatcher10Dispatcher11guest_enterEPNS_7ContextEm # guest_enter
.global _ZN10dispatcher10Dispatcher10guest_exitEv # guest_exit
.global _ZN10dispatcher10Dispatcher10fault_exitEPKc # fault_exit

# address of the context in reg
.macro capture_context reg
	sd ra, 0x00(\reg)
	sd sp, 0x08(\reg)
	sd gp, 0x10(\reg)
	sd tp, 0x18(\reg)
	sd t0, 0x20(\reg)
	sd t1, 0x28(\reg)
	sd t2, 0x30(\reg)
	sd fp, 0x38(\reg)
	sd s1, 0x40(\reg)
	sd a0, 0x48(\reg)
	sd a1, 0x50(\reg)
	sd a2, 0x58(\reg)
	sd a3, 0x60(\reg)
	sd a4, 0x68(\reg)
	sd a5, 0x70(\reg)
	sd a6, 0x78(\reg)
	sd a7, 0x80(\reg)
	sd s2, 0x88(\reg)
	sd s3, 0x90(\reg)
	sd s4, 0x98(\reg)
	sd s5, 0xA0(\reg)
	sd s6, 0xA8(\reg)
	sd s7, 0xB0(\reg)
	sd s8, 0xB8(\reg)
	sd s9, 0xC0(\reg)
	sd s10, 0xC8(\reg)
	sd s11, 0xD0(\reg)
	sd t3, 0xD8(\reg)
	sd t4, 0xE0(\reg)
	sd t5, 0xE8(\reg)
	sd t6, 0xF0(\reg)
.endm

# address of the context in reg (has to be a temporary register)
.macro restore_context reg
	ld ra, 0x00(\reg)
	ld sp, 0x08(\reg)
	ld gp, 0x10(\reg)
	ld tp, 0x18(\reg)
	ld fp, 0x38(\reg)
	ld s1, 0x40(\reg)
	ld a0, 0x48(\reg)
	ld a1, 0x50(\reg)
	ld a2, 0x58(\reg)
	ld a3, 0x60(\reg)
	ld a4, 0x68(\reg)
	ld a5, 0x70(\reg)
	ld a6, 0x78(\reg)
	ld a7, 0x80(\reg)
	ld s2, 0x88(\reg)
	ld s3, 0x90(\reg)
	ld s4, 0x98(\reg)
	ld s5, 0xA0(\reg)
	ld s6, 0xA8(\reg)
	ld s7, 0xB0(\reg)
	ld s8, 0xB8(\reg)
	ld s9, 0xC0(\reg)
	ld s10, 0xC8(\reg)
	ld s11, 0xD0(\reg)
.endm

# guest_exit
_ZN10dispatcher10Dispatcher10guest_exitEv:
	# move the exit code into t1
	mv t1, a0

	# t0 (host_context) = s11 (guest_context) + sizeof(Context)
	addi t0, s11, 248

	# restore the host context
	restore_context t0

	# move the exit code into the return value register and return
	mv a0, t1
	jalr zero, ra, 0

# guest_enter
_ZN10dispatcher10Dispatcher11guest_enterEPNS_7ContextEm:
	# store a pointer to the guest context
	mv t0, a0
	mv t3, a1

	# a0 = host_context (guest_context + sizeof(Context))
	addi a0, a0, 248

	# store host context
	capture_context a0

	# load guest context
	restore_context t0

	# call reroute_dynamic to translate the address in t3
	jalr ra, s9, 0

	jalr zero, t3, 0