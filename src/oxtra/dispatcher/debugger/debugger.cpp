#include "debugger.h"
#include <spdlog/spdlog.h>
#include "oxtra/dispatcher/dispatcher.h"

size_t debugger::DebuggerBatch::add(utils::riscv_instruction_t inst) {
	if (count >= codegen::codestore::max_riscv_instructions)
		dispatcher::Dispatcher::fault_exit("DebuggerBatch::add - buffer-overflow");
	riscv[count] = inst;
	return count++;
}

void debugger::DebuggerBatch::begin(const fadec::Instruction* inst, bool eob, uint8_t update, uint8_t require) {
	// clear the size and add the jump to the debug-entry
	count = 0;
	codegen::jump_table::jump_debugger(*this);
}

void debugger::DebuggerBatch::end() {
}

void debugger::DebuggerBatch::print() const {
	for (size_t i = 0; i < count; ++i)
		spdlog::trace("    [{:02}] = {}", i, decoding::parse_riscv(riscv[i]));
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
	if (break_point == halt_break) {
		halt = false;
	}

	cout << "Hello World! - from the debugger: 0x" << hex << break_point << endl;
}