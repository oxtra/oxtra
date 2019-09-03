.include "oxtra/dispatcher/dispatcher.s"

.global _ZN7codegen10jump_table13table_addressEv # jump_table
.global c_wrapper

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

# don't use the 'j' pseudo-instruction as it might unwrap to multiple instructions
.align 8
_ZN7codegen10jump_table13table_addressEv:
	jal zero, _ZN10dispatcher10Dispatcher15syscall_handlerEv
	jal zero, _ZN10dispatcher10Dispatcher14reroute_staticEv
	jal zero, _ZN10dispatcher10Dispatcher15reroute_dynamicEv
	jal zero, c_wrapper # debug-callback
