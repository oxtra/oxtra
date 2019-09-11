.global _ZN10dispatcher10Dispatcher11guest_enterEPNS_16ExecutionContextEmPPKc # guest_enter
.global _ZN10dispatcher10Dispatcher10guest_exitEl # guest_exit
.global _ZN10dispatcher10Dispatcher10fault_exitEPKcl # fault_exit
.global _ZN10dispatcher10Dispatcher14reroute_staticEv # reroute_static
.global _ZN10dispatcher10Dispatcher15reroute_dynamicEv # reroute_dynamic
.global _ZN10dispatcher10Dispatcher15syscall_handlerEv # syscall_handler

.section .text

# guest_enter
_ZN10dispatcher10Dispatcher11guest_enterEPNS_16ExecutionContextEmPPKc:
	# store a pointer to the guest context
	mv t0, a0
	mv t3, a1

	# a0 = host_context (guest_context + sizeof(Context))
	addi a0, a0, host_offset

	# t0 = guest_context
	addi t0, t0, guest_offset

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
	addi t0, s11, host_offset

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
	addi t0, s11, host_offset

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
	ld a0, codegen_offset(s11)
	mv a1, s0
	jal ra, _ZN7codegen13CodeGenerator9translateEm

	# write the new address into s0
	mv s0, a0

	# _codegen.update_basic_block(ra, translated_address);
	ld a0, codegen_offset(s11)
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
    ld a0, codegen_offset(s11)
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
	sd a0, guest_a0_offset(s11)

	# restore the guest context and return to caller
syscall_handled:
    restore_context s11
    ret
