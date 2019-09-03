.global _ZN7codegen10jump_table13table_addressEv # jump_table

# implement wrapper to call c function in register t4


# don't use the 'j' pseudo-instruction as it might unwrap to multiple instructions
.align 8
_ZN7codegen10jump_table13table_addressEv:
	jal zero, _ZN10dispatcher10Dispatcher15syscall_handlerEv
	jal zero, _ZN10dispatcher10Dispatcher14reroute_staticEv
	jal zero, _ZN10dispatcher10Dispatcher15reroute_dynamicEv