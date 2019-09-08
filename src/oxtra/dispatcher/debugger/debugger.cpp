#include "debugger.h"

#include "oxtra/codegen/jump-table/jump_table.h"

size_t debugger::DebuggerBatch::add(utils::riscv_instruction_t inst) {
	riscv[count] = inst;
	return count++;
}

void debugger::DebuggerBatch::end() {
}

//void debugger::DebuggerBatch::print() const {
//}

void debugger::DebuggerBatch::reset() {
	// clear the size and add the jump to the debug-entry
	count = 0;
	codegen::jump_table::jump_debugger(*this);
}

#include <iostream>

using namespace std;

debugger::Debugger::Debugger() {
	halt = true;
	bp_count = 4;
	memset(bp_array, 0, sizeof(uintptr_t) * 256);
	bp_array[0] = 0x4010a02064;
	bp_array[1] = 0x45400a02064;
	bp_array[2] = 0x4000a020c8;
	bp_array[3] = 0x4000a02068;
}

void debugger::Debugger::entry(dispatcher::ExecutionContext* context, uintptr_t break_point) {
	if(break_point == halt_break){
		halt = false;
	}

	cout << "Hello World! - from the debugger: 0x" << hex << break_point << endl;
}