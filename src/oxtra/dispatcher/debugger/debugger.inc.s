.global debug_entry

.section .text

# define the debug-entry, which invokes the debugging_entry-function
debug_entry:
	ret

	# ensure that a debugger exists


	# capture the context
	capture_context_full s11

	# call the debugging-function

