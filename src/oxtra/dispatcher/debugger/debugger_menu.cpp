#include "debugger.h"
#include <spdlog/spdlog.h>
#include <string>
#include <iostream>

#include "oxtra/dispatcher/dispatcher.h"

std::string debugger::Debugger::parse_input(utils::guest_addr_t address, dispatcher::ExecutionContext* context) {
	// define the constants
	static constexpr uint8_t arg_state_unused = 0xa0;
	static constexpr uint8_t arg_state_string = 0xb0;
	static constexpr uint8_t arg_state_number = 0xc0;
	static constexpr uint8_t arg_state_pos_rel = '+';
	static constexpr uint8_t arg_state_neg_rel = '-';
	static constexpr uint8_t arg_state_key = 0xd0;

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

	// check if the arguments can be parsed as numbers
	if (!arg_string[0].empty()) {
		// check if it might be a number
		if (parse_number(arg_string[0], &arg_state[0], arg_number[0])) {
			if (arg_state[0] == 0)
				arg_state[0] = arg_state_number;
		} else
			arg_state[0] = arg_state_string;

		// if its a string, check if it might be a key
		if (arg_state[0] == arg_state_string) {
			arg_key[0] = parse_key(arg_string[0]);
			if (arg_key[0] != DebugInputKey::none)
				arg_state[0] = arg_state_key;
		}

		// check if a second argument exists
		if (!arg_string[1].empty()) {
			// check if it might be a number
			if (parse_number(arg_string[1], &arg_state[1], arg_number[1])) {
				if (arg_state[1] == 0)
					arg_state[1] = arg_state_number;
			} else
				arg_state[1] = arg_state_string;

			// if its a string, check if it might be a key
			if (arg_state[1] == arg_state_string) {
				arg_key[1] = parse_key(arg_string[1]);
				if (arg_key[1] != DebugInputKey::none)
					arg_state[1] = arg_state_key;
			}
		}
	}

	// extract the argument;
	std::string argument;
	sstr >> argument;

	// handle the input-key
	switch (key) {
		case DebugInputKey::assembly:
			if (arg_state[0] == arg_state_number) {
				if (arg_number[0] == 0 || arg_number[0] > 96)
					return "assembly-count out of range [1;96]!";
				return print_assembly(address, _current, arg_number[0]);
			}
			return print_assembly(address, _current);


		case DebugInputKey::x86:
			_state &= ~DebugState::reg_riscv;
			return "set register-output to x86!";
		case DebugInputKey::riscv:
			_state |= DebugState::reg_riscv;
			return "set register-output to riscv!";
		case DebugInputKey::all:
			_state |= (DebugState::print_asm | DebugState::print_flags | DebugState::print_reg);
			return "all core-features enabled!";
		case DebugInputKey::breakpoint:
			if (arg_state[0] == arg_state_unused)
				return print_break_points();
			else if (arg_state[0] != arg_state_number && arg_state[0] != arg_state_neg_rel && arg_state[0] != arg_state_pos_rel)
				return "invalid break-point address!";
			else if (_bp_count == 256)
				return "limit of break-points reached (256)!";
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
		case DebugInputKey::continue_run:
		case DebugInputKey::run:
			if (arg_state[0] == arg_state_unused)
				return "";
			else if (arg_state[0] != arg_state_number)
				return "invalid execute-counter!";
			_state |= DebugState::await_counter;
			_bp_counter = arg_number[0];
			return "";
		case DebugInputKey::decimal:
			_state |= DebugState::reg_dec;
			return "set register-printing to dec!";
		case DebugInputKey::exit:
			dispatcher::Dispatcher::guest_exit(0);
			break;
		case DebugInputKey::endofblock:
			_state |= DebugState::await_eblock;
			return "";
		case DebugInputKey::fault:
			dispatcher::Dispatcher::fault_exit("the debugger exited via a fault.");
			break;
		case DebugInputKey::flags:
			return print_flags(context);
		case DebugInputKey::help:
			input = "all                                Enable registers,assembly with riscv and flags auto-print.\n";
			input += "assembly   asm                     Print the assembly with a maximum output of 12-instructions.\n";
			input += "assembly   asm  count              Print the assembly with a maximum output of count-instructions.\n";
			input += "break      bp                      Print the break-points.\n";
			input += "break      bp   (+-)addr           Add break-point with relative or absolute address.\n";
			input += "continue   c                       Continue execution until break-point.\n";
			input += "continue   c    count              Continue execution for count-instructions or until break-point.\n";
			input += "dec        dc                      Set the register print-type to decimal.\n";
			input += "exit                               Exit the program via dispatcher::guest_exit.\n";
			input += "eblock     eob                     Continue execution until end-of-block or break-point.\n";
			input += "fault                              Exit the program via dispatcher::fault_exit.\n";
			input += "flags      fg                      Print the flags.\n";
			input += "help                               Print this menu.\n";
			input += "hex        hx                      Set the register print-type to hexadecimal.\n";
			input += "logging    log                     Set the current logging-level (same as argument).\n";
			input += "remove     rbp  index              Remove one break-point with the given index.\n";
			input += "registers  reg                     List all of the registers with the current configuration.\n";
			input += "registers  reg  dec/hex riscv/x86  List all of the registers with a set configuration.\n";
			input += "riscv      rv                      Set the printed registers to riscv.\n";
			input += "run        r                       Continue execution until break-point.\n";
			input += "run        r    count              Continue execution for count-instructions or until break-point.\n";
			input += "sblock     sob                     Continue execution until start-of-block or break-point.\n";
			input += "step       s                       Step one x86-instruction.\n";
			input += "toggle     tg   assembly, asm      Toggle auto-print for assembly.\n";
			input += "toggle     tg   break, bp          Toggle auto-print for break-points.\n";
			input += "toggle     tg   flags, fg          Toggle auto-print for flags.\n";
			input += "toggle     tg   registers, reg     Toggle auto-print for registers.\n";
			input += "x86        x                       Set the printed registers to x86.";
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
			_state |= DebugState::await_sblock;
			return "";
		case DebugInputKey::toggle:
			if (arg_state[0] != arg_state_key)
				return "invalid argument for toggle!";
			else if (arg_key[0] == DebugInputKey::breakpoint) {
				_state ^= DebugState::print_bp;
				return "toggled break-points auto-print!";
			}
			else if (arg_key[0] == DebugInputKey::registers) {
				_state ^= DebugState::print_reg;
				return "toggled register auto-print!";
			}
			else if (arg_key[0] == DebugInputKey::assembly) {
				_state ^= DebugState::print_asm;
				return "toggled assembly auto-print!";
			}
			else if (arg_key[0] == DebugInputKey::flags) {
				_state ^= DebugState::print_flags;
				return "toggled flags auto-print!";
			}
			return "invalid argument for toggle!";
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
			break;
		case 'c':
			if (key == "continue" || key == "c")
				return DebugInputKey::continue_run;
			break;
		case 'd':
			if (key == "dec" || key == "dc")
				return DebugInputKey::decimal;
			break;
		case 'e':
			if (key == "exit")
				return DebugInputKey::exit;
			else if (key == "eblock" || key == "eob")
				return DebugInputKey::endofblock;
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
		case 's':
			if (key == "step" || key == "s")
				return DebugInputKey::step;
			else if (key == "sblock" || key == "sob")
				return DebugInputKey::startofblock;
			break;
		case 't':
			if (key == "toggle" || key == "tg")
				return DebugInputKey::toggle;
			break;
		case 'x':
			if (key == "x86" || key == "x")
				return DebugInputKey::x86;
			break;
	}
	return DebugInputKey::none;
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

std::string debugger::Debugger::print_assembly(utils::guest_addr_t guest, BlockEntry* entry, uint8_t limit) {
	// find the current index and compute the counters
	utils::guest_addr_t x86_address = entry->entry->x86_start;
	utils::host_addr_t riscv_address = entry->entry->riscv_start + 4;
	size_t index = 0;
	for (; index < entry->entry->instruction_count; index++) {
		if (x86_address == guest)
			break;
		x86_address += entry->entry->offsets[index].x86;
		riscv_address += entry->entry->offsets[index].riscv;
	}
	size_t riscv_count = (entry->entry->offsets[index].riscv >> 2u) - 1;

	// build the string
	std::string out_str("Assembly [x86]:");
	out_str.insert(out_str.size(), 50 - out_str.size(), ' ');
	out_str.append("Next instruction [riscv]:\n");

	// iterate through the instructions and write them to the string
	bool decode_failed = false;
	for (size_t i = 0; i < limit; i++) {
		// add the current address
		std::string line_buffer = "    [";
		line_buffer.append(print_number(x86_address, true));
		line_buffer.append("] ");

		// try to decode the instruction
		fadec::Instruction inst{};
		if(!decode_failed) {
			if (fadec::decode(reinterpret_cast<const uint8_t*>(x86_address), _elf.get_size(x86_address),
							  fadec::DecodeMode::decode_64, x86_address, inst) <= 0) {
				if (index < entry->entry->instruction_count)
					line_buffer = "failed to decode instruction";
				decode_failed = true;
			} else {
				char buffer[256];
				fadec::format(inst, buffer, 256);
				line_buffer.append(buffer);
			}
		}

		// check if this row contains a break-point
		if (!decode_failed) {
			for (size_t j = 0; j < _bp_count; j++) {
				if (_bp_x86_array[j] >= x86_address && _bp_x86_array[j] < x86_address + inst.get_size()) {
					line_buffer[1] = '*';
					break;
				}
			}
		}
		if (x86_address < entry->entry->x86_end)
			line_buffer[3] = '>';

		// pad the string
		if(line_buffer.size() < 50)
			line_buffer.insert(line_buffer.size(), 50 - line_buffer.size(), ' ');

		// update the x86-data
		if(!decode_failed) {
			x86_address += inst.get_size();
			index++;
		}

		// check any riscv-code exists
		if(i < riscv_count && i + 1 == limit)
			line_buffer.append("    ...");
		else if (i < riscv_count) {
			// set the pointer to the current riscv-instruction
			line_buffer.append((i == 0) ? " -> " : "    ");

			// decode the riscv-instruction
			std::string riscv_str = decoding::parse_riscv(reinterpret_cast<utils::riscv_instruction_t*>(riscv_address)[i]);
			if (riscv_str.size() <= 50)
				line_buffer.append(riscv_str);
			else
				line_buffer.append(riscv_str.c_str(), 50);
		}
		out_str.append(line_buffer);
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
	} else {
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
	} else {
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