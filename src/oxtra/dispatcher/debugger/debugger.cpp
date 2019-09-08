#include "debugger.h"
#include <spdlog/spdlog.h>


#include <iostream>

using namespace std;

void debugger::DebuggerBatch::print() const {
	for (size_t i = 1; i < count; ++i)
		spdlog::trace("    [{:02}] = {}", i - 1, decoding::parse_riscv(riscv[i]));
}

debugger::Debugger* debugger::Debugger::active_debugger = nullptr;

debugger::Debugger::Debugger() {
	// initialize the attributes
	halt = true;
	bp_count = 0;

	// mark this as the active debugger
	active_debugger = this;
}

debugger::Debugger::~Debugger() {
	// reset the active debugger
	if (active_debugger == this)
		active_debugger = nullptr;
}

void debugger::Debugger::begin_block(codegen::CodeBatch& batch){
	unused_parameter(batch);
}

void debugger::Debugger::insert(codegen::CodeBatch& batch, codegen::Instruction* inst){
	// check if a debugger exists
	if(!active_debugger)
		return;

	// add the instruction to jump to the debugger
	codegen::jump_table::jump_debugger(batch);
}

void debugger::Debugger::end_block(codegen::CodeBatch& batch, codegen::codestore::BlockEntry* block){

}

void debugger::Debugger::entry(dispatcher::ExecutionContext* context, uintptr_t break_point) {
	if (break_point == halt_break) {
		halt = false;

		// create some temporary breakpoints
		bp_count = 4;
		bp_array[0] = 0x4010a02064;
		bp_array[1] = 0x45400a02064;
		bp_array[2] = 0x4000a020c8;
		bp_array[3] = 0x4000a02068;
	}

	cout << "Hello World! - from the debugger: 0x" << hex << break_point << endl;
}