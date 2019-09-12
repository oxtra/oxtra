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
	std::cout << "debug>";
	std::string input;
	getline(std::cin, input);

	// verify the input
	if (input.empty())
		return "invalid input!";

	// split the input
	std::stringstream sstr(input);
	input.clear();
	std::string arg_string[3];
	uintptr_t arg_number[3] = {0, 0, 0};
	uint8_t arg_state[3] = {arg_state_unused, arg_state_unused};
	DebugInputKey arg_key[3] = {DebugInputKey::none, DebugInputKey::none};
	sstr >> input >> arg_string[0] >> arg_string[1] >> arg_string[2];

	// lookup the key
	DebugInputKey key = parse_key(input);
	if (key == DebugInputKey::none)
		return "invalid input! Type \"help\" for help.";

	// parse the first argument
	if (parse_argument(arg_string[0], arg_state[0], arg_number[0], arg_key[0])) {
		if (parse_argument(arg_string[1], arg_state[1], arg_number[1], arg_key[1]))
			parse_argument(arg_string[2], arg_state[2], arg_number[2], arg_key[2]);
	}

	// handle the input-key
	std::string str;
	switch (key) {
		case DebugInputKey::help:
			if (arg_state[0] == arg_state_unused) {
				return "Type help and the command for more details.\n"
					   "all                                Enable auto-print for flags, registers, assembly and stack.\n"
					   "assembly   asm                     Print the assembly.\n"
					   "blocks     blk                     Print the basic blocks.\n"
					   "break      bp                      Print the break-points or set them.\n"
					   "config     cfg                     Configure features for the printing.\n"
					   "continue   cne                     Continue execution for a number of instructions.\n"
					   "crawl      c                       Step by one riscv-instruction.\n"
					   "dec        dc                      Set the register print-type to decimal.\n"
					   "disable    dis                     Disable auto-print.\n"
					   "enable     en                      Enable auto-print.\n"
					   "exit                               Exit the program via dispatcher::guest_exit.\n"
					   "end        eob                     Continue execution until end-of-block or break-point.\n"
					   "fault                              Exit the program via dispatcher::fault_exit.\n"
					   "flags      fg                      Print the flags.\n"
					   "help                               Print this menu.\n"
					   "hex        hx                      Set the register print-type to hexadecimal.\n"
					   "logging    log                     Set the current logging-level (same as argument).\n"
					   "quit       q                       Disable the debugger and continue normal execution.\n"
					   "read       rd                      Read from memory.\n"
					   "remove     rbp                     Remove one break-point.\n"
					   "registers  reg                     List all of the registers with the current configuration.\n"
					   "run        r                       Continue execution until given instruction has been reached.\n"
					   "stack      stk                     Print the stack.\n"
					   "start      sob                     Continue execution until start-of-block or break-point.\n"
					   "step       s                       Step by one x86-instruction.";
			} else if (arg_state[0] != arg_state_key)
				return "invalid argument!";
			switch (arg_key[0]) {
				case DebugInputKey::assembly:
					return "assembly limit                Print limit-number of instructions.";
				case DebugInputKey::blocks:
					return "blocks index                  Print assembly of block.\n"
						   "blocks index limit            Print assembly of block and limit."
						   "blocks index limit inst       Print assembly of block and limit with instruction.";
				case DebugInputKey::breakpoint:
					return "break address                 Set a break-point.\n"
						   "break relative                Set a break-point relative to address.\n"
						   "break sob                     Set a break-point at beginning of current block.\n"
						   "break eob                     Set a break-point at end of current block.\n"
						   "break sob index               Set a break-point at beginning of index-block.\n"
						   "break eob index               Set a break-point at end of index-block.";
				case DebugInputKey::config:
					return "config asm limit              Set the assembly-limit.\n"
						   "config stack limit            Set the stack-limit.\n"
						   "config riscv/rv               Set the registers to riscv.\n"
						   "config x86/x                  Set the registers to x86.\n";
				case DebugInputKey::continue_run:
					return "continue count                Continue execution for count-instructions.";
				case DebugInputKey::disable:
					return "disable assembly              Disable auto-print of assembly.\n"
						   "disable break                 Disable auto-print of breakpoints.\n"
						   "disable registers             Disable auto-print of registers.\n"
						   "disable flags                 Disable auto-print of flags.\n"
						   "disable stack                 Disable auto-print of stack.\n"
						   "disable blocks                Disable auto-print of blocks.\n";
				case DebugInputKey::enable:
					return "enable assembly               Enable auto-print of assembly.\n"
						   "enable break                  Enable auto-print of breakpoints.\n"
						   "enable registers              Enable auto-print of registers.\n"
						   "enable flags                  Enable auto-print of flags.\n"
						   "enable stack                  Enable auto-print of stack.\n"
						   "enable blocks                 Enable auto-print of blocks.\n";
				case DebugInputKey::read:
					return "read address                  Read one quad-word from the address.\n"
						   "read address count            Read count-quad-words from the address.";
				case DebugInputKey::run:
					return "run address                   Continue execution until address.\n"
						   "run relative                  Continue execution until relative-address.";
				case DebugInputKey::registers:
					return "registers riscv               Print riscv-registers.\n"
						   "registers x86                 Print x86-registers.\n"
						   "registers riscv hex           Print riscv-registers in hexadecimal.\n"
						   "registers x86 hex             Print x86-registers in hexadecimal.\n"
						   "registers riscv dec           Print riscv-registers in decimal.\n"
						   "registers x86 dec             Print x86-registers in decimal.\n";
				case DebugInputKey::stack:
					return "stack limit                   Print the stack with given limit.\n"
						   "stack limit address           Print the stack with given limit and at given address.\n"
						   "stack limit relative          Print the stack with given limit and relative to rsp.\n";
				default:
					return "no additional features!";
			}
		case DebugInputKey::all:
			_state |= (DebugState::print_asm | DebugState::print_flags | DebugState::print_reg | DebugState::print_stack);
			return "all core-features enabled!";
		case DebugInputKey::assembly:
			if (arg_state[0] == arg_state_number) {
				if (arg_number[0] < 3 || arg_number[0] > 96)
					return "assembly-limit out of range [3;96]!";
				return print_assembly(address, context->guest.ra, _current, arg_number[0]);
			}
			return print_assembly(address, context->guest.ra, _current, _inst_limit);
		case DebugInputKey::blocks:
			if (arg_state[0] == arg_state_number) {
				if (arg_number[0] >= _blocks.size())
					return "block-index out of range!";
				BlockEntry* block = &_blocks[arg_number[0]];
				if (arg_state[1] == arg_state_number) {
					if (arg_number[1] < 3 || arg_number[1] > 96)
						return "assembly-limit out of range [3;96]!";
					if (arg_state[2] == arg_state_number) {
						if (arg_number[2] >= block->entry->instruction_count)
							return "instruction-index out of range!";
						uintptr_t x86_addr = block->entry->x86_start;
						uintptr_t riscv_addr = block->entry->riscv_start + 4;
						for (size_t i = 0; i < arg_number[2]; i++) {
							x86_addr += block->entry->offsets[i].x86;
							riscv_addr += block->entry->offsets[i].riscv;
						}
						return print_assembly(x86_addr, riscv_addr, block, arg_number[1]);
					}
					return print_assembly(block->entry->x86_start, block->entry->riscv_start, block, arg_number[1]);
				}
				return print_assembly(block->entry->x86_start, block->entry->riscv_start, block, _inst_limit);
			}
			return print_blocks();
		case DebugInputKey::breakpoint:
			if (arg_state[0] == arg_state_number || arg_state[1] == arg_state_neg_rel || arg_state[2] == arg_state_pos_rel
				|| (arg_state[0] == arg_state_key &&
					(arg_key[0] == DebugInputKey::startofblock || arg_key[0] == DebugInputKey::endofblock))) {
				if (_bp_count == 255)
					return "limit of break-points reached (255)!";
				if (arg_state[0] == arg_state_number) {
					if (arg_state[0] == arg_state_pos_rel)
						address += arg_number[0];
					else if (arg_state[0] == arg_state_neg_rel)
						address -= arg_number[0];
					else
						address = arg_number[0];
				} else {
					BlockEntry* block = _current;
					if (arg_state[1] == arg_state_number) {
						if (arg_state[1] >= _blocks.size())
							return "block-index out of range!";
						block = &_blocks[arg_number[1]];
					}
					if (arg_key[0] == DebugInputKey::startofblock)
						address = block->entry->x86_start;
					else
						address = block->entry->x86_end - block->entry->offsets[block->entry->instruction_count - 1].x86;
				}
				_bp_x86_array[_bp_count] = address;
				translate_break_point(_bp_count);
				for (size_t i = 0; i < _bp_count; i++) {
					if (_bp_x86_array[i] == _bp_x86_array[_bp_count])
						return "break-point already set!";
				}
				_bp_count++;
				return "break-point set!";
			}
			return print_break_points();
		case DebugInputKey::config:
			if (arg_state[0] != arg_state_key)
				return "invalid config-attribute!";
			else if (arg_key[0] == DebugInputKey::assembly) {
				if (arg_state[1] != arg_state_number)
					return "invalid assembly-limit!";
				if (arg_number[1] < 3 || arg_number[1] > 96)
					return "assembly-limit out of range [3;96]!";
				_inst_limit = arg_number[1];
				return "successfully configured!";
			} else if (arg_key[0] == DebugInputKey::stack) {
				if (arg_state[1] != arg_state_number)
					return "invalid stack-limit!";
				if (arg_number[1] < 2 || arg_number[1] > 128)
					return "stack-limit out of range [2;128]!";
				_stack_limit = arg_number[1];
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
			if (arg_state[0] == arg_state_number) {
				_state |= DebugState::await_counter;
				_run_break = arg_number[0];
			}
			return "";
		case DebugInputKey::crawl:
			if (!_riscv_enabled)
				return "riscv-stepping is disabled!";
			_step_riscv = true;
			_state |= DebugState::await_step;
			return "";
		case DebugInputKey::decimal:
			_state |= DebugState::reg_dec;
			return "set register-printing to dec!";
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
			} else if (arg_key[0] == DebugInputKey::stack) {
				_state &= ~DebugState::print_stack;
				return "disabled stack auto-print!";
			} else if (arg_key[0] == DebugInputKey::blocks) {
				_state &= ~DebugState::print_blocks;
				return "disabled blocks auto-print!";
			}
			return "invalid argument for disable!";
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
			} else if (arg_key[0] == DebugInputKey::stack) {
				_state |= DebugState::print_stack;
				return "enabled stack auto-print!";
			} else if (arg_key[0] == DebugInputKey::blocks) {
				_state |= DebugState::print_blocks;
				return "enabled blocks auto-print!";
			}
			return "invalid argument for enable!";
		case DebugInputKey::exit:
			dispatcher::Dispatcher::guest_exit(0);
			break;
		case DebugInputKey::endofblock:
			_state |= DebugState::await_eob;
			return "";
		case DebugInputKey::fault:
			dispatcher::Dispatcher::fault_exit("the debugger exited via a fault.");
			break;
		case DebugInputKey::flags:
			return print_flags(context);
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
		case DebugInputKey::quit:
			context->debugger = nullptr;
			return "";
		case DebugInputKey::read:
			if ((_state & DebugState::read_warning) == 0) {
				_state |= DebugState::read_warning;
				return "Warning: the debugger won't protect from Segmentation-faults.";
			}
			if (arg_state[0] != arg_state_number)
				return "invalid address!";
			if (arg_state[1] == arg_state_number) {
				if (arg_number[1] == 0 || arg_number[1] > 128)
					return "count out of range [1;128]!";
				return print_memory(arg_number[0], arg_number[1]);
			}
			return print_memory(arg_number[0], 1);
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
		case DebugInputKey::run:
			if (arg_state[0] == arg_state_neg_rel || arg_state[0] == arg_state_pos_rel ||
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
			}
			return "";
		case DebugInputKey::stack:
			if (arg_state[0] == arg_state_number) {
				if (arg_number[0] < 2 || arg_number[0] > 128)
					return "stack-limit out of range [2;128]!";
				if (arg_state[1] == arg_state_number || arg_state[1] == arg_state_pos_rel ||
					arg_state[1] == arg_state_neg_rel) {
					if (arg_state[1] == arg_state_pos_rel)
						arg_number[1] = context->guest.map.rsp + arg_number[1];
					else if (arg_state[1] == arg_state_neg_rel)
						arg_number[1] = context->guest.map.rsp - arg_number[1];
					return print_stack(arg_number[1], context, arg_number[0]);
				}
				return print_stack(context->guest.map.rsp, context, arg_number[0]);
			}
			return print_stack(context->guest.map.rsp, context, _stack_limit);
		case DebugInputKey::startofblock:
			_state |= DebugState::await_sob;
			return "";
		case DebugInputKey::step:
			_state |= DebugState::await_step;
			return "";
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
			else if (key == "blocks" || key == "blk")
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
		case 'q':
			if (key == "quit" || key == "q")
				return DebugInputKey::quit;
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
			else if (key == "read" || key == "rd")
				return DebugInputKey::read;
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