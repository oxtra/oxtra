.global debug_entry

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


	# check if breakpoint reached


	# capture the context
	capture_context_full s11

	# call the debugging-entry-function
	jal ra, _ZN8debugger8Debugger5entryEPN10dispatcher16ExecutionContextE

	# restore the context and return to the execution
	restore_context_full s11
	ret
