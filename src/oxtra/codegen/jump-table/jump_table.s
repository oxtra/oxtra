.global _ZN7codegen10jump_table13table_addressEv # jump_table
.global c_wrapper
.global unsupported_overflow
.global unsupported_carry

.include "oxtra/dispatcher/dispatcher.s"

.section .rodata
unsupported_overflow_string: .string "the overflow flag has been read from an Instruction which doesn't implement it."
unsupported_carry_string: .string "the carry flag has been read from an Instruction which doesn't implement it."

.section .text

# implement wrapper to call c function in register t4 -> t4
# prototype: static uintptr_t c_wrapper(Context* context)
c_wrapper:
	# capture the context
	capture_context s11

	# invoke the function
    mv a0, s11
    jalr ra, t4

    # move the return-value into register t4
    mv t4, a0

	# restore the guest context and return to caller
    restore_context s11
    ret


# throw an error if the overflow is not supported
unsupported_overflow:
	# set the string-pointer and the exit-code
	la a0, unsupported_overflow_string
	li a1, -1

	# call the fault-exit
	j _ZN10dispatcher10Dispatcher10fault_exitEPKcl


# throw an error if the carry is not supported
unsupported_carry:
	# set the string-pointer and the exit-code
	la a0, unsupported_carry_string
	li a1, -1

	# call the fault-exit
	j _ZN10dispatcher10Dispatcher10fault_exitEPKcl


# don't use the 'j' pseudo-instruction as it might unwrap to multiple instructions
.align 8
_ZN7codegen10jump_table13table_addressEv:
	jal zero, _ZN10dispatcher10Dispatcher15syscall_handlerEv
	jal zero, _ZN10dispatcher10Dispatcher14reroute_staticEv
	jal zero, _ZN10dispatcher10Dispatcher15reroute_dynamicEv
	jal zero, c_wrapper # debug-callback
	jal zero, unsupported_carry
	jal zero, unsupported_overflow
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

	jal zero, carry_sub_8
	jal zero, carry_sub_16
	jal zero, carry_sub_32
	jal zero, carry_sub_64

	jal zero, overflow_add_8
	jal zero, overflow_add_16
	jal zero, overflow_add_32
	jal zero, overflow_add_64

	jal zero, overflow_adc_8
	jal zero, overflow_adc_16
	jal zero, overflow_adc_32
	jal zero, overflow_adc_64

	jal zero, overflow_add_8
	jal zero, overflow_add_16
	jal zero, overflow_add_32
	jal zero, overflow_add_64