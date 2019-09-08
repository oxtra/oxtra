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

void debugger::Debugger::entry(dispatcher::ExecutionContext* context){
	cout << "Hello World! - from the debugger" << endl;
}