#include "debugger.h"
#include <spdlog/spdlog.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sstream>
#include <string>


#include <iostream>
#include <oxtra/dispatcher/execution_context.h>

using namespace std;

void debugger::DebuggerBatch::print() const {
	for (size_t i = 1; i < count; ++i)
		spdlog::trace("    [{:02}] = {}", i - 1, decoding::parse_riscv(riscv[i]));
}

debugger::Debugger* debugger::Debugger::active_debugger = nullptr;

debugger::Debugger::Debugger() {
	// initialize the attributes
	_halt = true;
	_bp_count = 0;
	_state = DebugState::none;
	_bp_counter = 0;
	_current = nullptr;

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
	unused_parameter(inst);

	// check if a debugger exists
	if (!active_debugger)
		return;

	// add the instruction to jump to the debugger
	codegen::jump_table::jump_debugger(batch);
}

void debugger::Debugger::end_block(codegen::CodeBatch& batch, codegen::codestore::BlockEntry* block) {
	unused_parameter(batch);

	// check if a debugger exists
	if (!active_debugger)
		return;

	// compute the end of the riscv-instructions
	utils::host_addr_t riscv_end = block->riscv_start;
	for (size_t i = 0; i < block->instruction_count; i++)
		riscv_end += block->offsets[i].riscv;

	// add the block to the list
	active_debugger->_blocks.emplace_back(block, riscv_end);

	// update the break-points
	active_debugger->update_break_points(active_debugger->_blocks.back());
}

void debugger::Debugger::entry(dispatcher::ExecutionContext* context, uintptr_t break_point) {
	// check if the debugger should be entered
	utils::guest_addr_t address = enter_break(break_point, context->guest.ra);
	if (address == 0)
		return;

	// enter the loop
	std::string menu_string = "";
	do {
		// print the separating line and the current address
		std::stringstream out;
		out << '\n' << std::string(100, '-') << '\n';
		out << "guest-address: " << print_number(address, true) << '\n';
		out << "host-address : " << print_number(context->guest.ra, true) << "\n\n";

		// print the registers
		if (_state & DebugState::print_reg)
			out << print_reg(context) << '\n';

		// print the flags
		if (_state & DebugState::print_flags)
			out << print_flags(context) << '\n';

		// print the assembly
		if (_state & (DebugState::print_x86 | DebugState::print_riscv))
			out << print_assembly(address, context->guest.ra) << "\n\n";

		// check if the menu has to be printed
		if (menu_string.empty()) {
			// print the reason for the break
			if (break_point != halt_break)
				out << "break-point hit! (index: " << dec << break_point << ")\n";
			if ((_state & DebugState::init) == 0) {
				out << "initial halt! Type \"help\" for a help-menu.\n";
				_state |= DebugState::init;
			}
			if (_bp_counter == 0 && (_state & DebugState::await_counter))
				out << "run-counter ellapsed.\n";
			if (_state & DebugState::await_step)
				out << "stepped by one instruction!\n";
			if (address == _current->entry->x86_start && (_state & DebugState::await_sblock))
				out << "beginning of basic-block reached!\n";
			else if (address == _current->entry->x86_end && (_state & DebugState::await_eblock))
				out << "ending of basic-block reached!\n";

			// clear the await-details and the break-point
			_state &= ~DebugState::await;
			break_point = halt_break;
		} else
			out << menu_string << '\n';

		// print the screen
		cout << out.str() << endl;

		// handle input
		menu_string = parse_input();
	} while (!menu_string.empty());

	// print the closing line
	cout << std::string(100, '-') << endl;

	// update the halt-flag
	_halt = (_state & DebugState::await) > 0;
}

utils::guest_addr_t debugger::Debugger::resolve_block(utils::host_addr_t address) {
	// check if a current basic block exists
	if (_current != nullptr) {
		if (address < _current->entry->riscv_start || address >= _current->riscv_end)
			_current = nullptr;
	}

	// resolve the new block, if necessary
	if (_current == nullptr) {
		// look for the basic-block
		for (auto& block : _blocks) {
			if (address >= block.entry->riscv_start && address < block.riscv_end) {
				_current = &block;
				break;
			}
		}
		if (_current == nullptr)
			return 0;
	}

	// resolve the x86-address
	utils::host_addr_t riscv_counter = _current->entry->riscv_start;
	utils::guest_addr_t x86_counter = _current->entry->x86_start;
	for (size_t i = 0; i < _current->entry->instruction_count; i++) {
		if (riscv_counter == address)
			return x86_counter;
		else if (riscv_counter > address)
			return x86_counter - _current->entry->offsets[i - 1].x86;
		riscv_counter += _current->entry->offsets[i].riscv;
		x86_counter += _current->entry->offsets[i].x86;
	}

	// return the last instruction, as this point can only be reached, if address points within one instruction
	return x86_counter - _current->entry->offsets[_current->entry->instruction_count - 1].x86;
}

utils::guest_addr_t debugger::Debugger::enter_break(uintptr_t break_point, utils::host_addr_t address) {
	// update the counter
	if (_state & DebugState::await_counter) {
		if (_bp_counter > 0)
			_bp_counter--;
		if (_bp_counter == 0)
			return resolve_block(address);
	}

	// check for a stepper
	if (_state & DebugState::await_step)
		return resolve_block(address);

	// check if a break-point has been hit or if this is the init-break
	if (break_point != halt_break || (_state & DebugState::init) == 0)
		return resolve_block(address);

	// check if a basic block has been reached
	if ((_state & DebugState::await_eblock) || (_state & DebugState::await_sblock)) {
		// resolve the address, which should also update the current block
		utils::guest_addr_t guest_addr = resolve_block(address);
		if (guest_addr == 0)
			return 0;

		// check if a beginning or an end has been reached
		if ((guest_addr == _current->entry->x86_start) && (_state & DebugState::await_sblock))
			return guest_addr;
		if ((guest_addr == _current->entry->x86_end - _current->entry->offsets[_current->entry->instruction_count - 1].x86) &&
			(_state & DebugState::await_eblock))
			return guest_addr;
	}
	return 0;
}

void debugger::Debugger::update_break_points(BlockEntry& block) {
	// iterate through the x86-break-points and check if one of them has been translated
	for (size_t i = 0; i < _bp_count; i++) {
		if (_bp_array[i] == 0) {
			// check if the instruction lies within the new block
			if (_bp_x86_array[i] >= block.entry->x86_start && _bp_x86_array[i] < block.entry->x86_end) {
				// initialize the counters (riscv + 4, as the first instruction is the jump to the debug-handler)
				uintptr_t x86_counter = block.entry->x86_start;
				uintptr_t riscv_counter = block.entry->riscv_start + 4;

				// iterate through the instructions and look for the one of the break-point
				for (size_t j = 0; j < block.entry->instruction_count; j++) {
					if (x86_counter == _bp_x86_array[i]) {
						_bp_array[i] = riscv_counter;
						break;
					} else if (x86_counter > _bp_x86_array[i]) {
						_bp_array[i] = riscv_counter - block.entry->offsets[j - 1].riscv;
						break;
					}
					riscv_counter += block.entry->offsets[j].riscv;
					x86_counter += block.entry->offsets[j].x86;
				}

				// if the break-point has not been set yet, it must lie within the last instruction
				_bp_array[i] = block.riscv_end - block.entry->offsets[block.entry->instruction_count - 1].x86;
			}
		}
	}
}

std::string debugger::Debugger::parse_input() {
	// await input
	cout << '>';
	std::string input;
	getline(cin, input);

	// verify the input
	if (input.empty())
		return "invalid input!";

	// iterate through the input and handle it
	std::stringstream sstr(input);
	while (!sstr.str().empty()) {
		sstr >> input;

		//
	}
}

std::string debugger::Debugger::print_number(uint64_t nbr, bool hex) {
	// build the string
	std::string str;
	if (hex) {
		// shift the mask and append the bytes
		str.append("0x");
		for (uint16_t i = 0; i < 16u; i++) {
			uint8_t digit = (nbr >> (60u - i * 4u)) & 0x000000000000000full;
			digit += digit < 10 ? '0' : ('a' - 10);
			str.push_back(digit);
		}
		return str;
	}

	// check if the number is zero
	if (nbr == 0)
		return "0";

	// check if the number is signed
	if (nbr & 0x8000000000000000ull) {
		nbr = (~nbr) + 1;

		// iterate through the string and insert the digits
		while (nbr > 0) {
			uint8_t digit = (nbr % 10) + '0';
			nbr /= 10;
			str.insert(0, 1, digit);
		}
		str.insert(0, 1, '-');
		return str;
	}

	// convert the number to a string
	while (nbr > 0) {
		uint8_t digit = (nbr % 10) + '0';
		nbr /= 10;
		str.insert(0, 1, digit);
	}
	return str;
}

std::string debugger::Debugger::print_reg(dispatcher::ExecutionContext* context) {
	static constexpr const char* riscv_map[] = {
			" ra", " sp", " gp", " tp", " t0", " t1", " t2", " s0", " s1",
			" a0", " a1", " a2", " a3", " a4", " a5", " a6", " a7", " s2",
			" s3", " s4", " s5", " s6", " s7", " s8", " s9", "s10", "s11",
			" t3", " t4", " t5", " t6"
	};

	// extract a boolean for the number-type
	bool hex = (_state & DebugState::reg_dec) == 0;

	// check if riscv or x86 is supposed to be printed
	std::string out_string = "registers:\n";
	if (_state & DebugState::reg_riscv) {
		for (size_t i = 0; i < 31; i++) {
			// build the string
			std::string temp_string = riscv_map[i];
			temp_string.push_back('=');
			temp_string.append(print_number(context->guest.reg[i], hex));

			// adjust the string and append the string to the output
			if (temp_string.size() < 25)
				temp_string.insert(temp_string.size(), 25 - temp_string.size(), ' ');
			if ((i % 4) == 0 && i > 0)
				out_string.push_back('\n');
			out_string.append(temp_string);
		}
		out_string.push_back('\n');
		return out_string;
	}

	// print the x86-registers
	for (size_t i = 0; i < 16; i++) {
		std::string temp_string;
		switch (i) {
			case 0:
				temp_string = "rax=" + print_number(context->guest.map.rax, hex);
				break;
			case 1:
				temp_string = "rcx=" + print_number(context->guest.map.rcx, hex);
				break;
			case 2:
				temp_string = "rdx=" + print_number(context->guest.map.rdx, hex);
				break;
			case 3:
				temp_string = "rbx=" + print_number(context->guest.map.rbx, hex);
				break;
			case 4:
				temp_string = "rsp=" + print_number(context->guest.map.rsp, hex);
				break;
			case 5:
				temp_string = "rbp=" + print_number(context->guest.map.rbp, hex);
				break;
			case 6:
				temp_string = "rsi=" + print_number(context->guest.map.rsi, hex);
				break;
			case 7:
				temp_string = "rdi=" + print_number(context->guest.map.rdi, hex);
				break;
			case 8:
				temp_string = "r08=" + print_number(context->guest.map.r8, hex);
				break;
			case 9:
				temp_string = "r09=" + print_number(context->guest.map.r9, hex);
				break;
			case 10:
				temp_string = "r10=" + print_number(context->guest.map.r10, hex);
				break;
			case 11:
				temp_string = "r11=" + print_number(context->guest.map.r11, hex);
				break;
			case 12:
				temp_string = "r12=" + print_number(context->guest.map.r12, hex);
				break;
			case 13:
				temp_string = "r13=" + print_number(context->guest.map.r13, hex);
				break;
			case 14:
				temp_string = "r14=" + print_number(context->guest.map.r14, hex);
				break;
			case 15:
			default:
				temp_string = "r15=" + print_number(context->guest.map.r15, hex);
				break;
		}

		// adjust the string and append the string to the output
		if (temp_string.size() < 25)
			temp_string.insert(temp_string.size(), 25 - temp_string.size(), ' ');
		if ((i % 4) == 0 && i > 0)
			out_string.push_back('\n');
		out_string.append(temp_string);
	}
	return out_string;
}

std::string debugger::Debugger::print_assembly(utils::guest_addr_t guest, utils::host_addr_t host) {
	return "";
}

std::string debugger::Debugger::print_flags(dispatcher::ExecutionContext* context) {
	return "";
}