#Documentation

####Why do we use a jump table to access host functions?
* Issue:
	* Because we wanted to implement the flag evaluation in software we needed a way to store the function pointers.
	* Not enough registers.
* Solutions:
	* Fit all functions in one page and store the middle of that page in a register for use in jalr.
		* Pro: Only one instruction generated for every invoked function.
		* Con: The functions must fit within a page boundary.
	* Store a function pointer table in the ExecutionContext.
		* Pro: Practically no limitation for function address and size.
		* Pro: Only one register required (for the ExecutionContext).
		* Pro: Creates a coherent unit of function pointers.
		* Con: Generates an additional load instruction for every invoked function.
	* Store a pointer to a jump table in another register.
		* Pro: Only one instruction generated for every invoked function.
		* Pro/Con: The table consists of jal's that 'only' allow for a 20-bit offset.
		* Con: We need another register to store the jump table pointer.