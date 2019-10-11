.global _ZN8debugger8Debugger17evaluate_overflowEPN10dispatcher16ExecutionContextEPNS2_7ContextE # evaluate_overflow
.global _ZN8debugger8Debugger14evaluate_carryEPN10dispatcher16ExecutionContextEPNS2_7ContextE # evaluate_carry
.global debug_entry
.global debug_entry_riscv

.section .text

# define the debug-entry, which invokes the debugging_entry-function
debug_entry:
	# store the current t0-register
	sd t0, guest_t0_offset(s11)

	# ensure that a debugger exists
	ld t0, debugger_offset(s11)
	bnez t0, debugger_attached
	ld t0, guest_t0_offset(s11)
	ret
	debugger_attached:

	# store the current t1 & t2-register
	sd t1, guest_t1_offset(s11)
	sd t2, guest_t2_offset(s11)

	# increase the instruction-counts
	ld t1, debug_count_x86(t0)
	addi t1, t1, 1
	sd t1, debug_count_x86(t0)
	ld t1, debug_count_riscv(t0)
	addi t1, t1, 1
	sd t1, debug_count_riscv(t0)

    # check if a signal-handler is registered
	ld t1, debug_sig_address(t0)
	beqz t1, debugger_no_signal

	# store the upcoming address and the registers
	sd ra, debug_sig_address(t0)
	addi t1, t0, debug_sig_registers
	capture_context_debug t1
	ld t1, guest_t0_offset(s11)
    sd t1, debug_sig_t0(t0)
    ld t1, guest_t1_offset(s11)
	sd t1, debug_sig_t1(t0)
    ld t1, guest_t2_offset(s11)
    sd t1, debug_sig_t2(t0)
	debugger_no_signal:

	# check if the execution is supposed to be halted
	lbu t2, debug_halt(t0)
	beqz t2, debugger_no_halt

	# set the t1-register to 0x400, which indicates to the entry, that the halt has been reached
	addi t1, zero, 0x400
	j debugger_enter

	# check if any break-points exist
	debugger_no_halt:
	lhu t1, debug_bp_count(t0)
	beqz t1, debugger_exit

	# iterate through the break-points and check if one has been hit
    addi t2, t0, debug_bp_array
    debugger_bp_iteration:
    beqz t1, debugger_exit

    # load the current entry and compare it to the ra-address
    ld t0, 0(t2)
    bne t0, ra, debugger_bp_iteration_no_enter

    # reload the debugger-pointer and compute the index
    ld t0, debugger_offset(s11)
    sub t1, t2, t0
    addi t1, t1, -debug_bp_array
    srli t1, t1, 3
    j debugger_enter

    # decrease the counter and increase the pointer
    debugger_bp_iteration_no_enter:
    addi t1, t1, -1
    addi t2, t2, 8
    j debugger_bp_iteration

    # restore the two temp-registers
	debugger_exit:
	ld t0, guest_t0_offset(s11)
	ld t1, guest_t1_offset(s11)
	ld t2, guest_t2_offset(s11)
	ret

	# capture the context
	debugger_enter:
    capture_context_debug s11

	# work on the stack after the sysv red zone
	addi sp, sp, -128

    # set the three arguments
    mv a0, t0	# Debugger
    mv a1, t1	# BreakPointEntry

	# call the debugging-entry-function
	jal ra, _ZN8debugger8Debugger5entryEm

	# restore the context and return to the execution
	restore_context_debug s11
	ret


# define the debug-entry for riscv, which invokes the debugging_entry-function
debug_entry_riscv:
	# store the current t0-register
	sd t0, guest_t0_offset(s11)

	# ensure that a debugger exists
	ld t0, debugger_offset(s11)
	bnez t0, debugger_riscv_attached
	ld t0, guest_t0_offset(s11)
	ret
	debugger_riscv_attached:

	# store the current t1-register
	sd t1, guest_t1_offset(s11)

	# increase the instruction-count
	ld t1, debug_count_riscv(t0)
	addi t1, t1, 1
	sd t1, debug_count_riscv(t0)

	# check if a signal-handler is registered
	ld t1, debug_sig_address(t0)
	beqz t1, debugger_riscv_no_signal

	# store the upcoming address and the registers
	sd ra, debug_sig_address(t0)
	addi t1, t0, debug_sig_registers
	capture_context_debug t1
	ld t1, guest_t0_offset(s11)
    sd t1, debug_sig_t0(t0)
    ld t1, guest_t1_offset(s11)
    sd t1, debug_sig_t1(t0)
    sd t2, debug_sig_t2(t0)
    debugger_riscv_no_signal:

	# check if the execution is supposed to be halted
	lbu t1, debug_step_riscv(t0)
	bnez t1, debugger_riscv_enter

	# restore the two temp-registers
	ld t0, guest_t0_offset(s11)
	ld t1, guest_t1_offset(s11)
	ret

	# capture the context
	debugger_riscv_enter:
    capture_context_debug s11
    sd t2, guest_t2_offset(s11)

	# work on the stack after the sysv red zone
	addi sp, sp, -128

    # set the three arguments
    mv a0, t0	# Debugger
    addi a1, zero, 0x600 # entry-type

	# call the debugging-entry-function
	jal ra, _ZN8debugger8Debugger5entryEm

	# restore the context and return to the execution
	restore_context_debug s11
	ret


# implement the function to compute the overflow
_ZN8debugger8Debugger17evaluate_overflowEPN10dispatcher16ExecutionContextEPNS2_7ContextE:
	# load the context-pointer into t0 and the context to t1
    mv t0, a1
    mv t1, a0

    # capture the context
    capture_context t0

    # restore the guest-context
    restore_context t1

    # call the jump-table-entry
    lhu t3, flag_info_overflow_operation(s11)
    add t3, t3, s10
    jalr ra, t3, 0

    # restore the context
    restore_context t0

    # move the result into a0
    mv a0, t4
    ret


# implement the function to compute the carry
_ZN8debugger8Debugger14evaluate_carryEPN10dispatcher16ExecutionContextEPNS2_7ContextE:
	# load the context-pointer into t0 and the context to t1
	mv t0, a1
	mv t1, a0

	# capture the context
	capture_context t0

	# restore the guest-context
	restore_context t1

	# call the jump-table-entry
	lhu t3, flag_info_carry_operation(s11)
	add t3, t3, s10
	jalr ra, t3, 0

	# restore the context
	restore_context t0

	# move the result into a0
	mv a0, t4
	ret
