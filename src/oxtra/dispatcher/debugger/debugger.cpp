#include "debugger.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <iostream>

#include "oxtra/dispatcher/dispatcher.h"

void debugger::DebuggerBatch::print() const {
	for (size_t i = 1; i < count; ++i)
		spdlog::trace("    [{:02}] = {}", i - 1, decoding::parse_riscv(riscv[i]));
}

debugger::Debugger* debugger::Debugger::active_debugger = nullptr;

debugger::Debugger::Debugger(const elf::Elf& elf) : _elf{elf} {
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

	// extract the current-current_block
	size_t current_index = active_debugger->_blocks.size() + 1;
	if (active_debugger->_current != nullptr) {
		for (size_t i = 0; i < active_debugger->_blocks.size(); i++) {
			if (&active_debugger->_blocks[i] == active_debugger->_current) {
				current_index = i;
				break;
			}
		}
	}

	// add the block to the list
	active_debugger->_blocks.emplace_back(block, riscv_end);

	// update the current block
	if (current_index != active_debugger->_blocks.size())
		active_debugger->_current = &active_debugger->_blocks[current_index];

	// update the break-points
	active_debugger->update_break_points(active_debugger->_blocks.back());
}

void debugger::Debugger::entry(dispatcher::ExecutionContext* context, uintptr_t break_point) {
	// check if the debugger should be entered
	utils::guest_addr_t address = enter_break(break_point, context->guest.ra);
	if (address == 0)
		return;

	// enter the loop
	std::string menu_string;
	do {
		// print the separating line and the current address
		std::stringstream out;
		out << '\n' << std::string(100, '-') << '\n';
		out << "guest-address: " << print_number(address, true) << '\n';
		out << "host-address : " << print_number(context->guest.ra, true) << "\n\n";

		// print the break-points
		if (_state & DebugState::print_bp)
			out << print_break_points() << '\n';

		// print the registers
		if (_state & DebugState::print_reg)
			out << print_reg(context, (_state & DebugState::reg_dec) == 0, _state & DebugState::reg_riscv) << '\n';

		// print the flags
		if (_state & DebugState::print_flags)
			out << print_flags(context) << '\n';

		// print the assembly
		if (_state & DebugState::print_asm)
			out << print_assembly(address, _current, _state & DebugState::asm_riscv) << '\n';

		// check if the menu has to be printed
		if (menu_string.empty()) {
			// print the reason for the break
			if (break_point != halt_break)
				out << "break-point hit! (index: " << std::dec << break_point << ")\n";
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
			else if (address == _current->entry->x86_end - _current->entry->offsets[_current->entry->instruction_count - 1].x86
					 && (_state & DebugState::await_eblock))
				out << "ending of basic-block reached!\n";

			// clear the await-details and the break-point
			_state &= ~DebugState::await;
			break_point = halt_break;
		} else
			out << menu_string << '\n';

		// print the screen
		std::cout << out.str() << std::endl;

		// handle input
		menu_string = parse_input(address, context);
	} while (!menu_string.empty());

	// print the closing line
	std::cout << '\n' << std::string(100, '-') << std::endl;

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

void debugger::Debugger::update_break_points(const BlockEntry& block) {
	// iterate through the x86-break-points and check if one of them has been translated
	for (size_t i = 0; i < _bp_count; i++) {
		if (_bp_array[i] != 0)
			continue;

		// check if the address lies within the new block
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
					_bp_x86_array[i] = x86_counter - block.entry->offsets[j - 1].x86;
					_bp_array[i] = riscv_counter - block.entry->offsets[j - 1].riscv;
					break;
				}
				riscv_counter += block.entry->offsets[j].riscv;
				x86_counter += block.entry->offsets[j].x86;
			}

			// if the break-point has not been set yet, it must lie within the last instruction
			if (_bp_array[i] == 0) {
				_bp_x86_array[i] = block.entry->x86_end - block.entry->offsets[block.entry->instruction_count - 1].x86;
				_bp_array[i] = block.riscv_end - block.entry->offsets[block.entry->instruction_count - 1].riscv;
			}
		}

		// check if a break-point already exists for this address
		if (_bp_array[i] == 0)
			continue;
		bool move = false;
		for (size_t j = 0; j < _bp_count; j++) {
			if (move) {
				_bp_x86_array[j - 1] = _bp_x86_array[j];
				_bp_array[j - 1] = _bp_array[j];
				continue;
			}
			if (i == j || _bp_x86_array[i] != _bp_x86_array[j])
				continue;
			move = true;
		}

		// check if an entry has been removed
		if (move)
			_bp_count--;
	}
}

void debugger::Debugger::translate_break_point(uint16_t index) {
	// iterate through the blocks and check if one of them suits the break-point
	utils::guest_addr_t address = _bp_x86_array[index];
	for (auto& block : _blocks) {
		// check if the address lies within the block
		if (address >= block.entry->x86_start && address < block.entry->x86_end) {
			// initialize the counters (riscv + 4, as the first instruction is the jump to the debug-handler)
			uintptr_t x86_counter = block.entry->x86_start;
			uintptr_t riscv_counter = block.entry->riscv_start + 4;

			// iterate through the instructions and look for the one of the break-point
			for (size_t j = 0; j < block.entry->instruction_count; j++) {
				if (x86_counter == address) {
					_bp_array[index] = riscv_counter;
					return;
				} else if (x86_counter > address) {
					_bp_x86_array[index] = x86_counter - block.entry->offsets[j - 1].x86;
					_bp_array[index] = riscv_counter - block.entry->offsets[j - 1].riscv;
					return;
				}
				riscv_counter += block.entry->offsets[j].riscv;
				x86_counter += block.entry->offsets[j].x86;
			}

			// if the break-point has not been set yet, it must lie within the last instruction
			_bp_x86_array[index] = block.entry->x86_end - block.entry->offsets[block.entry->instruction_count - 1].x86;
			_bp_array[index] = block.riscv_end - block.entry->offsets[block.entry->instruction_count - 1].riscv;
			return;
		}
	}

	// clear the break-point, as the block has not yet been translated
	_bp_array[index] = 0;
}

//TODO: add stack-print

std::string debugger::Debugger::parse_input(utils::guest_addr_t address, dispatcher::ExecutionContext* context) {
	// await input
	std::cout << '>';
	std::string input;
	getline(std::cin, input);

	// verify the input
	if (input.empty())
		return "invalid input!";

	// handle the input
	std::stringstream sstr(input);
	sstr >> input;

	// extract the argument;
	std::string argument;
	sstr >> argument;

	// switch the input and handle it
	switch (input[0]) {
		case 'a':
			if (input == "assembly" || input == "asm") {
				if (!argument.empty()) {
					if (argument == "riscv" || argument == "rv")
						return print_assembly(address, _current, true);
				}
				return print_assembly(address, _current, _state & DebugState::asm_riscv);
			} else if (input == "all") {
				_state |= (DebugState::print_asm | DebugState::print_flags | DebugState::print_reg | DebugState::asm_riscv);
				return "all core-features enabled!";
			}
			break;
		case 'b':
			if (input == "break" || input == "bp") {
				// check if the break-points should be listed or one added
				if (argument.empty())
					return print_break_points();

				// check if there is a break-point slot over
				if (_bp_count == 256)
					return "limit of break-points reached (256)!";

				// read the number
				uint8_t relative = 0;
				uintptr_t addr = 0;
				if (!parse_number(argument, &relative, addr))
					return "invalid break-point address!";
				if (relative == '+')
					addr += address;
				else if (relative == '-')
					addr -= address;

				// add the break-point
				_bp_x86_array[_bp_count] = addr;
				translate_break_point(_bp_count);

				// check if the break-point already exists
				for (size_t i = 0; i < _bp_count; i++) {
					if (_bp_x86_array[i] == _bp_x86_array[_bp_count])
						return "break-point already set!";
				}
				_bp_count++;
				return "break-point set!";
			}
			break;
		case 'c':
			if (input == "continue" || input == "c") {
				// check if it should continue without a counter
				if (argument.empty())
					return "";

				// read the number
				uintptr_t counter = 0;
				if (!parse_number(argument, nullptr, counter))
					return "invalid continue-counter!";

				// set the counter
				_bp_counter = counter;
				_state |= DebugState::await_counter;
				return "";
			}
			break;
		case 'd':
			if (input == "dec" || input == "dc") {
				_state |= DebugState::reg_dec;
				return "set register-printing to dec!";
			}
			break;
		case 'e':
			if (input == "exit")
				dispatcher::Dispatcher::guest_exit(0);
			else if (input == "eblock" || input == "eob") {
				// set the state
				_state |= DebugState::await_eblock;
				return "";
			}
			break;
		case 'f':
			if (input == "fault")
				dispatcher::Dispatcher::fault_exit("the debugger exited via a fault.");
			else if (input == "flags" || input == "fg")
				return print_flags(context);
			break;
		case 'h':
			if (input == "help") {
				std::string temp_str;
				temp_str += "all                                Enable registers,assembly with riscv and flags auto-print.\n";
				temp_str += "assembly   asm                     Print the assembly.\n";
				temp_str += "assembly   asm     riscv, rv       Print the assembly including the riscv-code.\n";
				temp_str += "break      bp                      Print the break-points.\n";
				temp_str += "break      bp      (+-)addr        Add break-point with relative or absolute address.\n";
				temp_str += "continue   c                       Continue execution until break-point.\n";
				temp_str += "continue   c       count           Continue execution for count-instructions or until break-point.\n";
				temp_str += "dec        dc                      Set the register print-type to decimal.\n";
				temp_str += "exit                               Exit the program via dispatcher::guest_exit.\n";
				temp_str += "eblock     eob                     Continue execution until end-of-block or break-point.\n";
				temp_str += "fault                              Exit the program via dispatcher::fault_exit.\n";
				temp_str += "flags      fg                      Print the flags.\n";
				temp_str += "help                               Print this menu.\n";
				temp_str += "hex        hx                      Set the register print-type to hexadecimal.\n";
				temp_str += "logging    log                     Set the current logging-level (same as argument).\n";
				temp_str += "remove     rbp     index           Remove one break-point with the given index.\n";
				temp_str += "registers  reg                     List all of the registers with the current configuration.\n";
				temp_str += "registers  reg     dec, dc         List all of the registers in decimal.\n";
				temp_str += "registers  reg     hex, hx         List all of the registers in hexadecimal.\n";
				temp_str += "registers  reg     riscv, rv       List all of the riscv-registers.\n";
				temp_str += "registers  reg     x86, x          List all of the x86-registers.\n";
				temp_str += "run        r                       Continue execution until break-point.\n";
				temp_str += "run        r       count           Continue execution for count-instructions or until break-point.\n";
				temp_str += "sblock     sob                     Continue execution until start-of-block or break-point.\n";
				temp_str += "step       s                       Step one x86-instruction.\n";
				temp_str += "toggle     tg      assembly, asm   Toggle auto-print for assembly.\n";
				temp_str += "toggle     tg      atype, at       Toggle the assembly-type (x86-only/both).\n";
				temp_str += "toggle     tg      break, bp       Toggle auto-print for break-points.\n";
				temp_str += "toggle     tg      flags, fg       Toggle auto-print for flags.\n";
				temp_str += "toggle     tg      registers, reg  Toggle auto-print for registers.\n";
				temp_str += "toggle     tg      rtype, rt       Toggle the register-type (riscv/x86).";
				return temp_str;
			} else if (input == "hex" || input == "hx") {
				_state &= ~DebugState::reg_dec;
				return "set register-printing to hex!";
			}
			break;
		case 'l':
			if (input == "logging" || input == "log") {
				// read the number
				uintptr_t level;
				if (!parse_number(argument, nullptr, level))
					return "invalid logging-level!";
				if (level > 6)
					return "logging-level out of range!";
				switch (level) {
					case 0:
						spdlog::set_level(spdlog::level::level_enum::trace);
						break;
					case 1:
						spdlog::set_level(spdlog::level::level_enum::debug);
						break;
					case 2:
						spdlog::set_level(spdlog::level::level_enum::info);
						break;
					case 3:
						spdlog::set_level(spdlog::level::level_enum::warn);
						break;
					case 4:
						spdlog::set_level(spdlog::level::level_enum::err);
						break;
					case 5:
						spdlog::set_level(spdlog::level::level_enum::critical);
						break;
					case 6:
						spdlog::set_level(spdlog::level::level_enum::off);
						break;
				}
				return "logging-level set!";
			}
			break;
		case 'r':
			if (input == "remove" || input == "rbp") {
				// read the number
				uintptr_t index;
				if (!parse_number(argument, nullptr, index))
					return "invalid break-point index!";

				// remove the index
				if (index >= _bp_count)
					return "break-point index out of range!";

				for (uint16_t i = index + 1; i < _bp_count; i++) {
					_bp_array[i - 1] = _bp_array[i];
					_bp_x86_array[i - 1] = _bp_x86_array[i];
				}
				_bp_count--;
				return "break-point removed!";
			} else if (input == "registers" || input == "reg") {
				// check if a hex/dec-style has been selected
				if (!argument.empty()) {
					if (argument == "dec" || argument == "dc")
						return print_reg(context, false, _state & DebugState::reg_riscv);
					else if (argument == "hex" || argument == "hx")
						return print_reg(context, true, _state & DebugState::reg_riscv);
					else if (argument == "riscv" || argument == "rv")
						return print_reg(context, (_state & DebugState::reg_dec) == 0, true);
					else if (argument == "x86" || argument == "x")
						return print_reg(context, (_state & DebugState::reg_dec) == 0, false);
				}
				return print_reg(context, (_state & DebugState::reg_dec) == 0, _state & DebugState::reg_riscv);
			} else if (input == "run" || input == "r") {
				// check if it should continue without a counter
				if (argument.empty())
					return "";

				// read the number
				uintptr_t counter = 0;
				if (!parse_number(argument, nullptr, counter))
					return "invalid run-counter!";

				// set the counter
				_bp_counter = counter;
				_state |= DebugState::await_counter;
				return "";
			}
			break;
		case 's':
			if (input == "step" || input == "s") {
				_state |= DebugState::await_step;
				return "";
			} else if (input == "sblock" || input == "sob") {
				_state |= DebugState::await_sblock;
				return "";
			}
			break;
		case 't':
			if (input == "toggle" || input == "tg") {
				if (argument == "break" || argument == "bp") {
					_state ^= DebugState::print_bp;
					return "toggled bp auto-print!";
				}
				if (argument == "rtype" || argument == "rt") {
					_state ^= DebugState::reg_riscv;
					return "toggled register riscv-style!";
				}
				if (argument == "registers" || argument == "reg") {
					_state ^= DebugState::print_reg;
					return "toggled register auto-print!";
				}
				if (argument == "assembly" || argument == "asm") {
					_state ^= DebugState::print_asm;
					return "toggled assembly auto-print!";
				}
				if (argument == "atype" || argument == "at") {
					_state ^= DebugState::asm_riscv;
					return "toggled assembly riscv-style!";
				}
				if (argument == "flags" || argument == "fg") {
					_state ^= DebugState::print_flags;
					return "toggled flags auto-print!";
				}
			}
			break;
	}

	// this point should only be reached, if the input has not been handled
	return "invalid input! Type \"help\" for help.";
}

bool debugger::Debugger::parse_number(std::string string, uint8_t* relative, uintptr_t& number) {
	// check if the string is relative
	if (string.empty())
		return false;
	if (relative) {
		if (string[0] == '-' || string[0] == '+') {
			relative[0] = string[0];
			string.erase(0, 1);
		} else
			relative[0] = 0;
	}

	// check if the string is empty
	if (string.empty())
		return false;

	// check if the number is a hex-number
	bool hex = false;
	if (string.size() > 1) {
		if (string[0] == '0' && string[1] == 'x') {
			hex = true;
			string.erase(0, 2);
		}
	}

	// parse the number
	number = 0;
	for (size_t i = 0; i < string.size(); i++) {
		if (string[i] >= '0' && string[i] <= '9')
			number = number * (hex ? 16 : 10) + (string[i] - '0');
		else if (string[i] >= 'a' && string[i] <= 'f' && hex)
			number = number * 16 + (string[i] - 'a' + 10);
		else
			return false;
	}
	return true;
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

std::string debugger::Debugger::print_reg(dispatcher::ExecutionContext* context, bool hex, bool riscv) {
	static constexpr const char* riscv_map[] = {
			" ra", " sp", " gp", " tp", " t0", " t1", " t2", " s0", " s1",
			" a0", " a1", " a2", " a3", " a4", " a5", " a6", " a7", " s2",
			" s3", " s4", " s5", " s6", " s7", " s8", " s9", "s10", "s11",
			" t3", " t4", " t5", " t6"
	};

	// check if riscv or x86 is supposed to be printed
	std::string out_string = "registers:\n";
	if (riscv) {
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
	out_string.push_back('\n');
	return out_string;
}

std::string debugger::Debugger::print_assembly(utils::guest_addr_t guest, BlockEntry* entry, bool riscv) {
	// find the current index and compute the counters
	utils::guest_addr_t x86_counter = entry->entry->x86_start;
	utils::host_addr_t riscv_counter = entry->entry->riscv_start + 4;
	size_t index = 0;
	for (; index < entry->entry->instruction_count; index++) {
		if (x86_counter == guest)
			break;
		x86_counter += entry->entry->offsets[index].x86;
		riscv_counter += entry->entry->offsets[index].riscv;
	}
	size_t riscv_count = (entry->entry->offsets[index].riscv >> 2u) - 1;

	// build the string
	std::string out_str("Assembly [x86]:");
	if (riscv) {
		out_str.insert(out_str.size(), 50 - out_str.size(), ' ');
		out_str.append("Next x86-instruction [riscv]:");
	}
	out_str.push_back('\n');

	// iterate through the instructions and write them to the string
	const auto loop_limit = ((riscv_count < 12 || !riscv) ? 12 : riscv_count);
	for (size_t i = 0; i < loop_limit; i++) {
		// try to decode the instruction
		std::string x86_buffer;
		fadec::Instruction inst{};
		bool decode_failed = false;
		if (fadec::decode(reinterpret_cast<const uint8_t*>(x86_counter), _elf.get_size(x86_counter),
						  fadec::DecodeMode::decode_64, x86_counter, inst) <= 0) {
			if (index < entry->entry->instruction_count)
				x86_buffer = "failed to decode instruction";
			else if (!riscv || i >= riscv_count)
				break;
			else
				x86_buffer.insert(0, 50, ' ');
			decode_failed = true;
		} else {
			char buffer[256];
			fadec::format(inst, buffer, 256);
			x86_buffer.append(buffer);
		}

		// check if this row contains a break-point
		std::string temp_str;
		if (!decode_failed) {
			for (size_t j = 0; j < _bp_count; j++) {
				if (_bp_x86_array[j] >= x86_counter && _bp_x86_array[j] < x86_counter + inst.get_size()) {
					temp_str.append(" * ");
					break;
				}
			}
		}
		if (temp_str.empty())
			temp_str.append("   ");
		if(x86_counter < entry->entry->x86_end)
			temp_str.push_back('>');
		else
			temp_str.push_back(' ');

		// add the current address
		temp_str.push_back('[');
		temp_str.append(print_number(x86_counter, true));
		temp_str.append("] ");

		// add the x86-instruction
		if (temp_str.size() + x86_buffer.size() > 50)
			temp_str.append(x86_buffer.c_str(), 50 - temp_str.size());
		else {
			temp_str.append(x86_buffer);
			temp_str.insert(temp_str.size(), 50 - temp_str.size(), ' ');
		}

		// update the x86-data
		x86_counter += inst.get_size();
		index++;

		// check any riscv-code exists
		if (riscv && i < riscv_count) {
			if (temp_str.size() < 50)
				temp_str.insert(temp_str.size(), 50 - temp_str.size(), ' ');

			// pad the string
			temp_str.append((i == 0) ? " -> " : "    ");

			// decode the riscv-instruction
			std::string riscv_str = decoding::parse_riscv(reinterpret_cast<utils::riscv_instruction_t*>(riscv_counter)[i]);
			if (riscv_str.size() <= 50)
				temp_str.append(riscv_str);
			else
				temp_str.append(riscv_str.c_str(), 50);
		}
		out_str.append(temp_str);
		out_str.push_back('\n');
	}
	return out_str;
}

std::string debugger::Debugger::print_flags(dispatcher::ExecutionContext* context) {
	// build the string
	std::string out_str = "flags:\n";

	// append the zero-flag
	std::string temp_str((context->flag_info.zero_value == 0) ? " ZF: 1" : " ZF: 0");
	temp_str.insert(temp_str.size(), 15 - temp_str.size(), ' ');
	out_str.append(temp_str);

	// append the sign-flag
	temp_str = " SF: ";
	if ((context->flag_info.sign_value >> context->flag_info.sign_size) == 1)
		temp_str.push_back('1');
	else
		temp_str.push_back('0');
	temp_str.insert(temp_str.size(), 15 - temp_str.size(), ' ');
	out_str.append(temp_str);

	// append the parity-flag
	temp_str = " PF: ";
	uint8_t temp = (context->flag_info.parity_value & 0x0fu) ^(context->flag_info.parity_value >> 4u);
	temp = (temp & 0x03u) ^ (temp >> 2u);
	temp = (temp & 0x01u) ^ (temp >> 1u);
	temp_str.append((temp == 0) ? "1" : "0");
	temp_str.insert(temp_str.size(), 15 - temp_str.size(), ' ');
	out_str.append(temp_str);

	// append the carry-flag
	temp_str = " CF: ";
	if (context->flag_info.carry_operation == static_cast<uint16_t>(codegen::jump_table::Entry::unsupported_carry) * 4) {
		temp_str.append("inv:");
		temp_str.append(reinterpret_cast<const char*>(context->flag_info.carry_pointer));
	}
	else {
		dispatcher::ExecutionContext::Context temp_context;
		temp_str.push_back('0' + evaluate_carry(context, &temp_context));
	}
	temp_str.insert(temp_str.size(), 15 - temp_str.size(), ' ');
	out_str.append(temp_str);

	// append the overflow-flag
	temp_str = " OF: ";
	if (context->flag_info.overflow_operation == static_cast<uint16_t>(codegen::jump_table::Entry::unsupported_overflow) * 4) {
		temp_str.append("inv:");
		temp_str.append(reinterpret_cast<const char*>(context->flag_info.overflow_pointer));
	}
	else {
		dispatcher::ExecutionContext::Context temp_context;
		temp_str.push_back('0' + evaluate_overflow(context, &temp_context));
	}
	temp_str.insert(temp_str.size(), 15 - temp_str.size(), ' ');
	out_str.append(temp_str);
	out_str.push_back('\n');
	return out_str;
}

std::string debugger::Debugger::print_break_points() {
	// iterate through the break-points and print them
	std::string out_string = "break-points:\n";
	for (size_t i = 0; i < _bp_count; i++) {
		// build the string
		std::string temp_string = print_number(i, false);
		while (temp_string.size() < 3)
			temp_string.insert(0, 1, ' ');
		temp_string.push_back('=');
		temp_string.append(print_number(_bp_x86_array[i], true));

		// adjust the string and append the string to the output
		if (temp_string.size() < 25)
			temp_string.insert(temp_string.size(), 25 - temp_string.size(), ' ');
		if ((i % 4) == 0 && i > 0)
			out_string.push_back('\n');
		out_string.append(temp_string);
	}
	if ((_bp_count & 0x03) != 0)
		out_string.push_back('\n');
	return out_string;
}