.global _ZN7codegen10jump_table13table_addressEv # jump_table

.include "oxtra/dispatcher/assembly_defines.inc.s"
.include "oxtra/dispatcher/dispatcher.inc.s"
.include "oxtra/debugger/debugger.inc.s"
.include "oxtra/codegen/instructions/arithmetic/flags_carry.inc.s"
.include "oxtra/codegen/instructions/arithmetic/flags_overflow.inc.s"

.section .text

# implement wrapper to call c function in register t4 -> t4
# prototype: static uintptr_t c_wrapper(ExecutionContext* context)
c_wrapper:
	# capture the context
	capture_context_high_level s11

	# work on the stack after the sysv red zone
	addi sp, sp, red_zone

	# invoke the function
    mv a0, s11
    jalr ra, t4

    # move the return-value into register t4
    mv t4, a0

	# restore the guest context and return to caller
	restore_context_high_level s11
    ret


# throw an error if the overflow is not supported
unsupported_overflow:
	# push ")\0" onto the stack
	addi sp, sp, -3
	addi t0, zero, 0
	sb t0, 2(sp)
	addi t0, zero, ']'
	sb t0, 1(sp)

	# load the error-string into a0, and count the length of it and find the end
	# a0 = pointer into the string
	# t1 = length of the string
	ld a0, flag_info_overflow_pointer(s11)
	addi t1, zero, 0
overflow_label_1:
	lb t0, 0(a0)
	beqz t0, overflow_label_2
	addi a0, a0, 1
	addi t1, t1, 1
	j overflow_label_1
overflow_label_2:

	# iterate through the string and write it onto the stack
	beqz t1, overflow_label_3
	addi a0, a0, -1
	lb t0, 0(a0)
	sb t0, 0(sp)
	addi sp, sp, -1
	addi t1, t1, -1
	j overflow_label_2
overflow_label_3:

	# append the closing bracket
	addi t0, zero, '['
	sb t0, 0(sp)
	addi sp, sp, -1

	# load the out-string into a0, and count the length of it and find the end
	# a0 = pointer into the string
	# t1 = length of the string
	la a0, unsupported_overflow_string
	addi t1, zero, 0
overflow_label_4:
	lb t0, 0(a0)
	beqz t0, overflow_label_5
	addi a0, a0, 1
	addi t1, t1, 1
	j overflow_label_4
overflow_label_5:

	# iterate through the string and write it onto the stack
	beqz t1, overflow_label_6
	addi a0, a0, -1
	lb t0, 0(a0)
	sb t0, 0(sp)
	addi sp, sp, -1
	addi t1, t1, -1
	j overflow_label_5
overflow_label_6:

	# set the string-pointer to the stack-pointer and the exit-code
	addi a0, sp, 1
	li a1, -1

	# call the fault-exit
	j _ZN10dispatcher10Dispatcher10fault_exitEPKcl


# throw an error if the carry is not supported
unsupported_carry:
	# push ")\0" onto the stack
	addi sp, sp, -3
	addi t0, zero, 0
	sb t0, 2(sp)
	addi t0, zero, ']'
	sb t0, 1(sp)

	# load the error-string into a0, and count the length of it and find the end
	# a0 = pointer into the string
	# t1 = length of the string
	ld a0, flag_info_carry_pointer(s11)
	addi t1, zero, 0
carry_label_1:
	lb t0, 0(a0)
	beqz t0, carry_label_2
	addi a0, a0, 1
	addi t1, t1, 1
	j carry_label_1
carry_label_2:

	# iterate through the string and write it onto the stack
	beqz t1, carry_label_3
	addi a0, a0, -1
	lb t0, 0(a0)
	sb t0, 0(sp)
	addi sp, sp, -1
	addi t1, t1, -1
	j carry_label_2
carry_label_3:

	# append the closing bracket
	addi t0, zero, '['
	sb t0, 0(sp)
	addi sp, sp, -1

	# load the out-string into a0, and count the length of it and find the end
	# a0 = pointer into the string
	# t1 = length of the string
	la a0, unsupported_carry_string
	addi t1, zero, 0
carry_label_4:
	lb t0, 0(a0)
	beqz t0, carry_label_5
	addi a0, a0, 1
	addi t1, t1, 1
	j carry_label_4
carry_label_5:

	# iterate through the string and write it onto the stack
	beqz t1, carry_label_6
	addi a0, a0, -1
	lb t0, 0(a0)
	sb t0, 0(sp)
	addi sp, sp, -1
	addi t1, t1, -1
	j carry_label_5
carry_label_6:

	# set the string-pointer to the stack-pointer and the exit-code
	addi a0, sp, 1
	li a1, -1

	# call the fault-exit
	j _ZN10dispatcher10Dispatcher10fault_exitEPKcl


# call a high-level function which computes the overflow-flag
high_level_overflow:
	# load the address into t4 and call the c-wrapper
	ld t4, flag_info_overflow_pointer(s11)
	j c_wrapper


# call a high-level function which computes the carry-flag
high_level_carry:
	# load the address into t4 and call the c-wrapper
	ld t4, flag_info_carry_pointer(s11)
	j c_wrapper


# don't use the 'j' pseudo-instruction as it might unwrap to multiple instructions
.align 8
_ZN7codegen10jump_table13table_addressEv:
	jal zero, _ZN10dispatcher10Dispatcher15syscall_handlerEv
	jal zero, _ZN10dispatcher10Dispatcher14reroute_staticEv
	jal zero, _ZN10dispatcher10Dispatcher15reroute_dynamicEv
	jal zero, _ZN10dispatcher10Dispatcher14reroute_returnEv
	jal zero, debug_entry
	jal zero, debug_entry_riscv
	jal zero, c_wrapper
	jal zero, _ZN7codegen22UnsupportedInstruction30handle_unsupported_instructionEm
	jal zero, unsupported_carry
	jal zero, unsupported_overflow
	jal zero, high_level_carry
	jal zero, high_level_overflow
	jal zero, carry_clear
	jal zero, carry_set
	jal zero, overflow_clear
	jal zero, overflow_set

	jal zero, carry_add_8
	jal zero, carry_add_16
	jal zero, carry_add_32
	jal zero, carry_add_64

	jal zero, carry_adc_8
	jal zero, carry_adc_16
	jal zero, carry_adc_32
	jal zero, carry_adc_64

	jal zero, carry_neg_8
	jal zero, carry_neg_16
	jal zero, carry_neg_32
	jal zero, carry_neg_64

	jal zero, overflow_add_8
	jal zero, overflow_add_16
	jal zero, overflow_add_32
	jal zero, overflow_add_64

	jal zero, overflow_adc_8
	jal zero, overflow_adc_16
	jal zero, overflow_adc_32
	jal zero, overflow_adc_64

	jal zero, overflow_add_pos_8
	jal zero, overflow_add_pos_16
	jal zero, overflow_add_pos_32
	jal zero, overflow_add_pos_64

	jal zero, overflow_add_neg_8
	jal zero, overflow_add_neg_16
	jal zero, overflow_add_neg_32
	jal zero, overflow_add_neg_64

	jal zero, overflow_sub_8
	jal zero, overflow_sub_16
	jal zero, overflow_sub_32
	jal zero, overflow_sub_64

	jal zero, overflow_inc_8
	jal zero, overflow_inc_16
	jal zero, overflow_inc_32
	jal zero, overflow_inc_64

	jal zero, overflow_dec_neg_8
	jal zero, overflow_dec_neg_16
	jal zero, overflow_dec_neg_32
	jal zero, overflow_dec_neg_64
