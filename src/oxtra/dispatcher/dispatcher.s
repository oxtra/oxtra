.global _ZN10dispatcher10Dispatcher11guest_enterEPNS_16ExecutionContextEmPPKc # guest_enter
.global _ZN10dispatcher10Dispatcher10guest_exitEl # guest_exit
.global _ZN10dispatcher10Dispatcher10fault_exitEPKcl # fault_exit
.global _ZN10dispatcher10Dispatcher14reroute_staticEv # reroute_static
.global _ZN10dispatcher10Dispatcher15reroute_dynamicEv # reroute_dynamic
.global _ZN10dispatcher10Dispatcher15syscall_handlerEv # syscall_handler

# address of the context in reg
.macro capture_context reg
	sd ra, 0x00(\reg)
	sd sp, 0x08(\reg)
	sd gp, 0x10(\reg)
	sd tp, 0x18(\reg)
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

.section .rodata
reroute_static_fmt: .string "reroute_static: 0x{0:x}"
reroute_dynamic_fmt: .string "reroute_dynamic: 0x{0:x}"

.section .text

# guest_enter
_ZN10dispatcher10Dispatcher11guest_enterEPNS_16ExecutionContextEmPPKc:
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
	call _ZN10dispatcher10Dispatcher15reroute_dynamicEv
	jalr zero, t3, 0


# guest_exit
_ZN10dispatcher10Dispatcher10guest_exitEl:
	# move the exit code into t1
	mv t1, a0

	# t0 (host_context) = s11 (guest_context) + sizeof(Context)
	addi t0, s11, 248

	# restore the host context
	restore_context t0

	# clear the string-pointer in argument 2
	xor t2, t2, t2
	sd t2, 0(a2)

	# move the exit-code into a0 and return to the point where guest_enter was called
	mv a0, t1
	ret


# fault_exit
_ZN10dispatcher10Dispatcher10fault_exitEPKcl:
	# move the string-pointer into t1 and the exit-code to t2
	mv t1, a0
	mv t2, a1

	# t0 (host_context) = s11 (guest_context) + sizeof(Context)
	addi t0, s11, 248

	# restore the host context
	restore_context t0

	# move the string-pointer to the address-pointer in argument 2
	sd t1, 0(a2)

	# move the exit-code into a0 and return to the point where guest_enter was called
	mv a0, t2
	ret


# reroute_static
_ZN10dispatcher10Dispatcher14reroute_staticEv:
	# capture the guest context
	capture_context s11

	# store the callers address (needed for update_basic_block)
	mv s1, ra

	# t3 might be changed by upcoming function calls, which is why we back it up
	mv s0, t3

	# spdlog::info("reroute_static: 0x{0:x}")
	# Speedlog requires a reference on the value.
	# To implement this, we push the value to display on the stack.
	# Afterwards we can just pass the stackpointer to SpeedLog.
	la a0, reroute_static_fmt
	addi sp, sp, -8
	sd s0, 0(sp)
	mv a1, sp
	jal ra, _ZN6spdlog4infoIJmEEEvPKcDpRKT_
	addi sp, sp, 8

	# _codegen.translate(t3)
	ld a0, 496(s11)
	mv a1, s0
	jal ra, _ZN7codegen13CodeGenerator9translateEm

	# write the new address into s0
	mv s0, a0

	# _codegen.update_basic_block(ra, translated_address);
	ld a0, 496(s11)
	mv a1, s1
	mv a2, s0
	jal ra, _ZN7codegen13CodeGenerator18update_basic_blockEmm

	# s0 will be overridden by restore_context so we have to save the translated address
	mv t3, s0

	# restore the guest context
	restore_context s11
	jalr zero, t3, 0


# reroute_dynamic
_ZN10dispatcher10Dispatcher15reroute_dynamicEv:
	# capture the guest context
	capture_context s11

	# t3 might be changed by upcoming function calls, which is why we back it up
	mv s0, t3

	# spdlog::info("reroute_dynamic: 0x{0:x}")
	# Speedlog requires a reference on the value.
	# To implement this, we push the value to display on the stack.
	# Afterwards we can just pass the stackpointer to SpeedLog.
	la a0, reroute_dynamic_fmt
	addi sp, sp, -8
	sd s0, 0(sp)
	mv a1, sp
	jal ra, _ZN6spdlog4infoIJmEEEvPKcDpRKT_
	addi sp, sp, 8

    # _codegen.translate(t3)
    ld a0, 496(s11)
    mv a1, s0
    jal ra, _ZN7codegen13CodeGenerator9translateEm

	# a0 will be overridden by restore_context so we have to save the translated address
	mv t3, a0

	# restore the guest context
	restore_context s11
	jalr zero, t3, 0


# syscall_handler
_ZN10dispatcher10Dispatcher15syscall_handlerEv:
	# capture the guest context
	capture_context s11

	# invoke virtualize_syscall and check if it should be forwarded
    mv a0, s11
    jal ra, _ZN10dispatcher10Dispatcher18virtualize_syscallEPKNS_16ExecutionContextE
    bltz a0, syscall_handled

	# arguments
	mv a7, a0 # syscall index -> a7
	ld a0, 0x70(s11) # arg0 (rdi)
	ld a1, 0x68(s11) # arg1 (rsi)
	ld a2, 0x60(s11) # arg2 (rdx)
	ld a3, 0x88(s11) # arg3 (r10)
	ld a4, 0x78(s11) # arg4 (r8)
	ld a5, 0x80(s11) # arg5 (r9)

	# execute the syscall and write the return value into _guest_context.a0
	ecall
	sd a0, 0x48(s11)

	# restore the guest context and return to caller
syscall_handled:
    restore_context s11
    ret
