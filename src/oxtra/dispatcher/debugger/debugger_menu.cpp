#include "debugger.h"
#include <spdlog/spdlog.h>
#include <string>
#include <iostream>

#include "oxtra/dispatcher/dispatcher.h"

// define the constants
static constexpr uint8_t arg_state_unused = 0xa0;
static constexpr uint8_t arg_state_string = 0xb0;
static constexpr uint8_t arg_state_number = 0xc0;
static constexpr uint8_t arg_state_pos_rel = '+';
static constexpr uint8_t arg_state_neg_rel = '-';
static constexpr uint8_t arg_state_key = 0xd0;

bool debugger::Debugger::parse_argument(std::string& str, uint8_t& state, uintptr_t& number, DebugInputKey& key) {
	// check if the argument exists
	if (str.empty()) {
		state = arg_state_unused;
		return false;
	}

	// check if it might be a number
	if (parse_number(str, &state, number)) {
		if (state == 0)
			state = arg_state_number;
		return true;
	}

	// check if the argument might be a key
	key = parse_key(str);
	if (key != DebugInputKey::none)
		state = arg_state_key;
	else
		state = arg_state_string;
	return true;
}

std::string debugger::Debugger::parse_input(utils::guest_addr_t address, dispatcher::ExecutionContext* context) {
	// await input
	std::cout << '>';
	std::string input;
	getline(std::cin, input);

	// verify the input
	if (input.empty())
		return "invalid input!";

	// split the input
	std::stringstream sstr(input);
	input.clear();
	std::string arg_string[2];
	uintptr_t arg_number[2] = {0, 0};
	uint8_t arg_state[2] = {arg_state_unused, arg_state_unused};
	DebugInputKey arg_key[2] = {DebugInputKey::none, DebugInputKey::none};
	while (!sstr.eof()) {
		// extract the key
		if (input.empty())
			sstr >> input;
		else if (arg_string[0].empty())
			sstr >> arg_string[0];
		else if (arg_string[1].empty())
			sstr >> arg_string[1];
		else
			break;
	}

	// lookup the key
	DebugInputKey key = parse_key(input);
	if (key == DebugInputKey::none)
		return "invalid input! Type \"help\" for help.";

	// parse the first argument
	if (parse_argument(arg_string[0], arg_state[0], arg_number[0], arg_key[0]))
		parse_argument(arg_string[1], arg_state[1], arg_number[1], arg_key[1]);

	// handle the input-key
	switch (key) {
		case DebugInputKey::assembly:
			if (arg_state[0] == arg_state_number) {
				if (arg_number[0] == 0 || arg_number[0] > 96)
					return "assembly-count out of range [1;96]!";
				return print_assembly(address, context->guest.ra, _current, arg_number[0]);
			}
			return print_assembly(address, context->guest.ra, _current, _inst_limit);
		case DebugInputKey::crawl:
			if (!_riscv_enabled)
				return "riscv-stepping is disabled!";
			_step_riscv = true;
			_state |= DebugState::await_step;
			return "";
		case DebugInputKey::all:
			_state |= (DebugState::print_asm | DebugState::print_flags | DebugState::print_reg);
			return "all core-features enabled!";
		case DebugInputKey::blocks:

		case DebugInputKey::stack:

		case DebugInputKey::breakpoint:
			if (arg_state[0] == arg_state_unused)
				return print_break_points();
			else if (arg_state[0] != arg_state_number && arg_state[0] != arg_state_neg_rel && arg_state[0] != arg_state_pos_rel)
				return "invalid break-point address!";
			else if (_bp_count == 255)
				return "limit of break-points reached (255)!";
			else {
				if (arg_state[0] == arg_state_pos_rel)
					arg_number[0] += address;
				else if (arg_state[0] == arg_state_neg_rel)
					arg_number[0] -= address;
				_bp_x86_array[_bp_count] = arg_number[0];
				translate_break_point(_bp_count);
				for (size_t i = 0; i < _bp_count; i++) {
					if (_bp_x86_array[i] == _bp_x86_array[_bp_count])
						return "break-point already set!";
				}
				_bp_count++;
				return "break-point set!";
			}
		case DebugInputKey::config:
			if (arg_state[0] != arg_state_key)
				return "invalid config-attribute!";
			else if (arg_key[0] == DebugInputKey::assembly) {
				if (arg_state[1] != arg_state_number)
					return "invalid assembly-count!";
				if (arg_number[1] == 0 || arg_number[1] > 96)
					return "assembly-count out of range [1;96]!";
				_inst_limit = arg_number[1];
				return "successfully configured!";
			} else if (arg_key[0] == DebugInputKey::x86) {
				_state &= ~DebugState::reg_riscv;
				return "successfully configured!";
			} else if (arg_key[0] == DebugInputKey::riscv) {
				_state |= DebugState::reg_riscv;
				return "successfully configured!";
			} else
				return "invalid config-attribute!";
		case DebugInputKey::continue_run:
			if (arg_state[0] == arg_state_unused)
				return "";
			else if (arg_state[0] != arg_state_number)
				return "invalid continue-counter!";
			_state |= DebugState::await_counter;
			_run_break = arg_number[0];
			return "";
		case DebugInputKey::run:
			if (arg_state[0] == arg_state_unused)
				return "";
			else if (arg_state[0] == arg_state_neg_rel || arg_state[0] == arg_state_pos_rel ||
					 arg_state[0] == arg_state_number) {
				if (arg_state[0] == arg_state_neg_rel)
					_bp_x86_array[_bp_count] = address - arg_number[0];
				else if (arg_state[0] == arg_state_pos_rel)
					_bp_x86_array[_bp_count] = address + arg_number[0];
				else
					_bp_x86_array[_bp_count] = arg_number[0];
				_run_break = _bp_count++;
				translate_break_point(_run_break);
				_state |= DebugState::temp_break;
				return "";
			}
			return "invalid run-argument!";
		case DebugInputKey::decimal:
			_state |= DebugState::reg_dec;
			return "set register-printing to dec!";
		case DebugInputKey::exit:
			dispatcher::Dispatcher::guest_exit(0);
			break;
		case DebugInputKey::endofblock:
			_state |= DebugState::await_eob;
			return "";
		case DebugInputKey::fault:
			dispatcher::Dispatcher::fault_exit("the debugger exited via a fault.");
			break;
		case DebugInputKey::quit:
			context->debugger = nullptr;
			return "";
		case DebugInputKey::flags:
			return print_flags(context);
		case DebugInputKey::help:

			//input += "assembly   asm  count              Print the assembly with a maximum output of count-instructions.\n";
			//input += "break      bp   (+-)addr           Add break-point with relative or absolute address.\n";
			//input += "config     cfg  asm     count      Set the default length for instruction-printing.\n";
			//input += "config     cfg  riscv rv           Configure registers to show the riscv-registers.\n";
			//input += "config     cfg  x86 x              Configure registers to show the x86-registers.\n";
			//input += "continue   cne  count              Continue execution for count-instructions or until break-point.\n";
			//input += "disable    dis  assembly, asm      Disable auto-print for assembly.\n";
			//input += "disable    dis  break, bp          Disable auto-print for break-points.\n";
			//input += "disable    dis  flags, fg          Disable auto-print for flags.\n";
			//input += "disable    dis  registers, reg     Disable auto-print for registers.\n";
			//input += "enable     en   assembly, asm      Enable auto-print for assembly.\n";
			//input += "enable     en   break, bp          Enable auto-print for break-points.\n";
			//input += "enable     en   flags, fg          Enable auto-print for flags.\n";
			//input += "enable     en   registers, reg     Enable auto-print for registers.\n";
			//input += "registers  reg  dec/hex riscv/x86  List all of the registers with a set configuration.\n";
			//input += "run        r    count              Continue execution for count-instructions or until break-point.\n";

			input = "Type help and the command for more details.\n";
			input += "all                                Enable auto-print for flags, registers, assembly and stack.\n";
			input += "assembly   asm                     Print the assembly.\n";
			input += "blocks     blk                     Print the basic blocks.\n";
			input += "break      bp                      Print the break-points.\n";
			input += "config     cfg                     Configure features for the printing.\n";
			input += "continue   cne                     Continue execution for a number of instructions.\n";
			input += "crawl      c                       Step by one riscv-instruction.\n";
			input += "dec        dc                      Set the register print-type to decimal.\n";
			input += "disable    dis                     Disable auto-print.\n";
			input += "enable     en                      Enable auto-print.\n";
			input += "exit                               Exit the program via dispatcher::guest_exit.\n";
			input += "end        eob                     Continue execution until end-of-block or break-point.\n";
			input += "fault                              Exit the program via dispatcher::fault_exit.\n";
			input += "flags      fg                      Print the flags.\n";
			input += "help                               Print this menu.\n";
			input += "hex        hx                      Set the register print-type to hexadecimal.\n";
			input += "logging    log                     Set the current logging-level (same as argument).\n";
			input += "quit       q                       Disable the debugger and continue normal execution.\n";
			input += "remove     rbp                     Remove one break-point.\n";
			input += "registers  reg                     List all of the registers with the current configuration.\n";
			input += "run        r                       Continue execution until given instruction has been reached.\n";
			input += "stack      stk                     Print the stack.\n";
			input += "start      sob                     Continue execution until start-of-block or break-point.\n";
			input += "step       s                       Step by one x86-instruction.";
			return input;
		case DebugInputKey::hexadecimal:
			_state &= ~DebugState::reg_dec;
			return "set register-printing to hex!";
		case DebugInputKey::logging:
			if (arg_state[0] != arg_state_number)
				return "invalid logging-level!";
			switch (arg_number[0]) {
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
				default:
					return "logging-level out of range!";
			}
			return "logging-level set!";
		case DebugInputKey::remove:
			if (arg_state[0] != arg_state_number)
				return "invalid break-point index!";
			else if (arg_number[0] >= _bp_count)
				return "break-point index out of range!";
			else {
				for (uint16_t i = arg_number[0] + 1; i < _bp_count; i++) {
					_bp_array[i - 1] = _bp_array[i];
					_bp_x86_array[i - 1] = _bp_x86_array[i];
				}
				_bp_count--;
				return "break-point removed!";
			}
		case DebugInputKey::registers:
			if (arg_state[0] == arg_state_key) {
				// extract the first feature
				bool riscv = _state & DebugState::reg_riscv;
				bool hex = (_state & DebugState::reg_dec) == 0;
				if (arg_key[0] == DebugInputKey::hexadecimal)
					hex = true;
				else if (arg_key[0] == DebugInputKey::decimal)
					hex = false;
				else if (arg_key[0] == DebugInputKey::riscv)
					riscv = true;
				else if (arg_key[0] == DebugInputKey::x86)
					riscv = false;
				else
					return print_reg(context, hex, riscv);

				// check if a second attribte exists
				if (arg_state[1] == arg_state_key) {
					if (arg_key[1] == DebugInputKey::hexadecimal)
						hex = true;
					else if (arg_key[1] == DebugInputKey::decimal)
						hex = false;
					else if (arg_key[1] == DebugInputKey::riscv)
						riscv = true;
					else if (arg_key[1] == DebugInputKey::x86)
						riscv = false;
				}
				return print_reg(context, hex, riscv);
			}
			return print_reg(context, (_state & DebugState::reg_dec) == 0, _state & DebugState::reg_riscv);
		case DebugInputKey::step:
			_state |= DebugState::await_step;
			return "";
		case DebugInputKey::startofblock:
			_state |= DebugState::await_sob;
			return "";
		case DebugInputKey::enable:
			if (arg_state[0] != arg_state_key)
				return "invalid argument for enable!";
			else if (arg_key[0] == DebugInputKey::breakpoint) {
				_state |= DebugState::print_bp;
				return "enabled break-points auto-print!";
			} else if (arg_key[0] == DebugInputKey::registers) {
				_state |= DebugState::print_reg;
				return "enabled register auto-print!";
			} else if (arg_key[0] == DebugInputKey::assembly) {
				_state |= DebugState::print_asm;
				return "enabled assembly auto-print!";
			} else if (arg_key[0] == DebugInputKey::flags) {
				_state |= DebugState::print_flags;
				return "enabled flags auto-print!";
			}
			return "invalid argument for enable!";
		case DebugInputKey::disable:
			if (arg_state[0] != arg_state_key)
				return "invalid argument for disable!";
			else if (arg_key[0] == DebugInputKey::breakpoint) {
				_state &= ~DebugState::print_bp;
				return "disabled break-points auto-print!";
			} else if (arg_key[0] == DebugInputKey::registers) {
				_state &= ~DebugState::print_reg;
				return "disabled register auto-print!";
			} else if (arg_key[0] == DebugInputKey::assembly) {
				_state &= ~DebugState::print_asm;
				return "disabled assembly auto-print!";
			} else if (arg_key[0] == DebugInputKey::flags) {
				_state &= ~DebugState::print_flags;
				return "disabled flags auto-print!";
			}
			return "invalid argument for disable!";
		default:
			break;
	}
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

debugger::Debugger::DebugInputKey debugger::Debugger::parse_key(std::string& key) {
	switch (key[0]) {
		case 'a':
			if (key == "assembly" || key == "asm")
				return DebugInputKey::assembly;
			else if (key == "all")
				return DebugInputKey::all;
			break;
		case 'b':
			if (key == "break" || key == "bp")
				return DebugInputKey::breakpoint;
			else if (key == "block" || key == "blk")
				return DebugInputKey::blocks;
			break;
		case 'c':
			if (key == "continue" || key == "cne")
				return DebugInputKey::continue_run;
			else if (key == "config" || key == "cfg")
				return DebugInputKey::config;
			else if (key == "crawl" || key == "c")
				return DebugInputKey::crawl;
			break;
		case 'd':
			if (key == "dec" || key == "dc")
				return DebugInputKey::decimal;
			else if (key == "disable" || key == "dis")
				return DebugInputKey::disable;
			break;
		case 'e':
			if (key == "exit")
				return DebugInputKey::exit;
			else if (key == "end" || key == "eob")
				return DebugInputKey::endofblock;
			else if (key == "enable" || key == "en")
				return DebugInputKey::enable;
			break;
		case 'f':
			if (key == "fault")
				return DebugInputKey::fault;
			else if (key == "flags" || key == "fg")
				return DebugInputKey::flags;
			break;
		case 'h':
			if (key == "help")
				return DebugInputKey::help;
			else if (key == "hex" || key == "hx")
				return DebugInputKey::hexadecimal;
			break;
		case 'l':
			if (key == "logging" || key == "log")
				return DebugInputKey::logging;
			break;
		case 'r':
			if (key == "remove" || key == "rbp")
				return DebugInputKey::remove;
			else if (key == "registers" || key == "reg")
				return DebugInputKey::registers;
			else if (key == "run" || key == "r")
				return DebugInputKey::run;
			else if (key == "riscv" || key == "rv")
				return DebugInputKey::riscv;
			break;
		case 'q':
			if (key == "quit" || key == "q")
				return DebugInputKey::quit;
			break;
		case 's':
			if (key == "step" || key == "s")
				return DebugInputKey::step;
			else if (key == "start" || key == "sob")
				return DebugInputKey::startofblock;
			else if (key == "stack" || key == "stk")
				return DebugInputKey::stack;
			break;
		case 'x':
			if (key == "x86" || key == "x")
				return DebugInputKey::x86;
			break;
	}
	return DebugInputKey::none;
}