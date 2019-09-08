#include "debugger.h"
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sstream>
#include <string>


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
	state = DebugState::none;
	debug_counter = 0;

	// mark this as the active debugger
	active_debugger = this;
}

debugger::Debugger::~Debugger() {
	// reset the active debugger
	if (active_debugger == this)
		active_debugger = nullptr;
}

void debugger::Debugger::begin_block(codegen::CodeBatch& batch) {
	unused_parameter(batch);
}

void debugger::Debugger::insert(codegen::CodeBatch& batch, codegen::Instruction* inst) {
	// check if a debugger exists
	if (!active_debugger)
		return;

	// add the instruction to jump to the debugger
	codegen::jump_table::jump_debugger(batch);
}

void debugger::Debugger::end_block(codegen::CodeBatch& batch, codegen::codestore::BlockEntry* block) {

}

void debugger::Debugger::entry(dispatcher::ExecutionContext* context, uintptr_t break_point) {
	if (break_point == halt_break) {
		if(debug_counter > 0) {
			debug_counter--;
			return;
		}
		halt = false;
	}

	// enter the loop
	bool debug_done;
	do {
		std::stringstream out;
		out << std::string(80, '-') << '\n';

		// print the initial message
		if (break_point != halt_break) {
			out << "break-point hit: " << print_number(bp_x86_array[break_point], true) << '\n';
		} else if ((state & DebugState::init) == 0) {
			out << "initial halt.\n";
			state |= DebugState::init;
		}
		else
			out << "run-counter ellapsed.\n";

		// print the registers
		if (state & DebugState::reg_print)
			out << print_reg(context);

		// print the screen
		cout << out.str() << endl;

		// await input
		cout << '>';
		std::string input;
		getline(cin, input);

		// parse the input
		debug_done = parse_input(input);
	} while (!debug_done);
}

bool debugger::Debugger::parse_input(std::string& input) {
	cout << "your input: " << input << endl;
	return true;
}

std::string debugger::Debugger::print_reg(dispatcher::ExecutionContext* context) {
	return "";
}

std::string debugger::Debugger::print_number(uint64_t nbr, bool hex) {
	return "";
}