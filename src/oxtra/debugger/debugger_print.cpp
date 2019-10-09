#include "debugger.h"
#include <string>
#include "oxtra/dispatcher/execution_context.h"
#include "oxtra/codegen/instruction.h"

std::string debugger::Debugger::print_number(uint64_t nbr, bool hex, uint8_t dec_digits, uint8_t dec_pad) {
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

	// check if the number is signed
	if (nbr & 0x8000000000000000ull) {
		nbr = (~nbr) + 1;

		// iterate through the string and insert the digits
		while (nbr > 0) {
			uint8_t digit = (nbr % 10) + '0';
			nbr /= 10;
			str.insert(0, 1, digit);
			if (dec_digits > 0)
				dec_digits--;
		}
		if (dec_digits > 0)
			str.insert(0, dec_digits, dec_pad);
		str.insert(0, 1, '-');
		return str;
	}

	// convert the number to a string
	if (nbr == 0) {
		str.push_back('0');
		dec_digits--;
	}
	while (nbr > 0) {
		uint8_t digit = (nbr % 10) + '0';
		nbr /= 10;
		str.insert(0, 1, digit);
		if (dec_digits > 0)
			dec_digits--;
	}
	if (dec_digits > 0)
		str.insert(0, dec_digits, dec_pad);
	return str;
}

std::string debugger::Debugger::print_reg(bool hex, bool riscv) {
	static constexpr const char* riscv_map[] = {
			" ra", " sp", " gp", " tp", " t0", " t1", " t2", " s0", " s1",
			" a0", " a1", " a2", " a3", " a4", " a5", " a6", " a7", " s2",
			" s3", " s4", " s5", " s6", " s7", " s8", " s9", "s10", "s11",
			" t3", " t4", " t5", " t6"
	};

	// check if riscv or x86 is supposed to be printed
	std::string out_string = "registers:\n";
	if (riscv) {
		for (int i = 0; i < 32; i++) {
			// check if its the zero-entry
			std::string temp_string;
			if (i == 0)
				temp_string = " zero-register";
			else {
				// build the string
				temp_string = riscv_map[i - 1];
				temp_string.push_back('=');
				temp_string.append(print_number(_context->guest.reg[i - 1], hex));
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

	// print the x86-registers
	for (size_t i = 0; i < 16; i++) {
		std::string temp_string;
		switch (i) {
			case 0:
				temp_string = "rax=" + print_number(_context->guest.map.rax, hex);
				break;
			case 1:
				temp_string = "rcx=" + print_number(_context->guest.map.rcx, hex);
				break;
			case 2:
				temp_string = "rdx=" + print_number(_context->guest.map.rdx, hex);
				break;
			case 3:
				temp_string = "rbx=" + print_number(_context->guest.map.rbx, hex);
				break;
			case 4:
				temp_string = "rsp=" + print_number(_context->guest.map.rsp, hex);
				break;
			case 5:
				temp_string = "rbp=" + print_number(_context->guest.map.rbp, hex);
				break;
			case 6:
				temp_string = "rsi=" + print_number(_context->guest.map.rsi, hex);
				break;
			case 7:
				temp_string = "rdi=" + print_number(_context->guest.map.rdi, hex);
				break;
			case 8:
				temp_string = "r08=" + print_number(_context->guest.map.r8, hex);
				break;
			case 9:
				temp_string = "r09=" + print_number(_context->guest.map.r9, hex);
				break;
			case 10:
				temp_string = "r10=" + print_number(_context->guest.map.r10, hex);
				break;
			case 11:
				temp_string = "r11=" + print_number(_context->guest.map.r11, hex);
				break;
			case 12:
				temp_string = "r12=" + print_number(_context->guest.map.r12, hex);
				break;
			case 13:
				temp_string = "r13=" + print_number(_context->guest.map.r13, hex);
				break;
			case 14:
				temp_string = "r14=" + print_number(_context->guest.map.r14, hex);
				break;
			case 15:
			default:
				temp_string = "r15=" + print_number(_context->guest.map.r15, hex);
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

std::string debugger::Debugger::print_assembly(utils::guest_addr_t guest, utils::host_addr_t host,
											   BlockEntry* entry, uint16_t limit) {
	utils::guest_addr_t guest_src = entry->entry->x86_start;
	utils::host_addr_t host_src = entry->entry->riscv_start + (_riscv_enabled ? 0 : 4);

	// compute the guest-index and the limit
	size_t guest_limit = entry->entry->instruction_count;
	size_t guest_index = 0;
	for (size_t i = 0; i < entry->entry->instruction_count; i++) {
		if (guest >= guest_src && guest < guest_src + entry->entry->offsets[i].x86) {
			guest_index = i;
			break;
		}
		guest_src += entry->entry->offsets[i].x86;
		host_src += entry->entry->offsets[i].riscv;
	}

	// compute the host-limit
	size_t host_limit = entry->entry->offsets[guest_index].riscv / 4;
	if (_riscv_enabled)
		host_limit /= 2;
	else
		host_limit--;

	// compute the host-index
	size_t host_index = 0;
	for (size_t i = 0; i < host_limit; i++) {
		if (host >= host_src && host < host_src + (_riscv_enabled ? 8 : 4)) {
			host_index = i;
			break;
		}
		host_src += _riscv_enabled ? 8 : 4;
	}

	// compute the start-indices
	size_t guest_start = 0;
	if (limit < guest_limit) {
		if (guest_index > (limit / 2))
			guest_start = guest_index - (limit / 2);
		if (guest_start + limit > guest_limit)
			guest_start = guest_limit - limit;
	}
	size_t host_start = 0;
	if (limit < host_limit) {
		if (host_index > (limit / 2))
			host_start = host_index - (limit / 2);
		if (host_start + limit > host_limit)
			host_start = host_limit - limit;
	}

	// compute the addresses
	guest_src = entry->entry->x86_start;
	host_src = entry->entry->riscv_start + 4;
	for (size_t i = 0; i < guest_start; i++)
		guest_src += entry->entry->offsets[i].x86;
	for (size_t i = 0; i < guest_index; i++)
		host_src += entry->entry->offsets[i].riscv;
	host_src += host_start * (_riscv_enabled ? 8 : 4);

	// build the string
	std::string out_str("Assembly [x86]: (");
	out_str.append(print_number(guest_index, false));
	out_str.push_back('/');
	out_str.append(print_number(guest_limit, false));
	out_str.push_back(')');
	out_str.insert(out_str.size(), 55 - out_str.size(), ' ');
	out_str.append("Next instruction [riscv]: (");
	out_str.append(print_number(host_index, false));
	out_str.push_back('/');
	out_str.append(print_number(host_limit, false));
	out_str.append(")\n");

	// iterate through the instructions and write them to the string
	for (size_t i = 0; i < limit; i++) {
		// add the x86-instruction
		std::string line_buffer;
		if ((i == 0 && guest_start > 0) || (i + 1 == limit && guest_start + i + 1 < guest_limit)) {
			line_buffer = "    ...";
			guest_src += entry->entry->offsets[guest_start + i].x86;
		} else if (guest_start + i < guest_limit) {
			if (guest_start + i == guest_index)
				line_buffer = "-> ";
			else
				line_buffer = "   ";

			// add break-points
			for (size_t j = 0; j < static_cast<size_t>(_bp_count - ((_state & DebugState::temp_break) ? 1 : 0)); j++) {
				if (_bp_x86_array[j] >= guest_src &&
					_bp_x86_array[j] < guest_src + entry->entry->offsets[guest_index + i].x86) {
					line_buffer.push_back('*');
					break;
				}
			}
			if (line_buffer.size() < 4)
				line_buffer.insert(line_buffer.size(), 4 - line_buffer.size(), ' ');

			// add the current address
			line_buffer.push_back('[');
			line_buffer.append(print_number(guest_src, true));
			line_buffer.append("] ");

			// try to decode the instruction
			fadec::Instruction inst{};
			if (fadec::decode(reinterpret_cast<const uint8_t*>(guest_src), _elf.get_size(guest_src),
							  fadec::DecodeMode::decode_64, guest_src, inst) <= 0) {
				line_buffer = "    failed to decode instruction";
			} else {
				char buffer[256];
				fadec::format(inst, buffer, 256);
				size_t len = strlen(buffer);
				if (len > 55 - line_buffer.size())
					line_buffer.append(buffer, 55 - line_buffer.size());
				else
					line_buffer.append(buffer);
			}

			// update the address
			guest_src += entry->entry->offsets[guest_start + i].x86;
		}

		// pad the string
		if (line_buffer.size() < 55)
			line_buffer.insert(line_buffer.size(), 55 - line_buffer.size(), ' ');

		// add the riscv-instruction
		if ((i == 0 && host_start > 0) || (i + 1 == limit && host_start + i + 1 < host_limit)) {
			line_buffer.append("     ...");
			host_src += _riscv_enabled ? 8 : 4;
		} else if (host_start + i < host_limit) {
			// set the pointer to the current riscv-instruction
			line_buffer.append((host_start + i == host_index) ? " ->  " : "     ");

			// decode the riscv-instruction
			std::string riscv_str = decoding::parse_riscv(reinterpret_cast<utils::riscv_instruction_t*>(host_src)[0]);
			if (riscv_str.size() <= 100 - line_buffer.size())
				line_buffer.append(riscv_str);
			else
				line_buffer.append(riscv_str.c_str(), 100 - line_buffer.size());

			// update the address
			host_src += _riscv_enabled ? 8 : 4;
		}
		out_str.append(line_buffer);
		out_str.push_back('\n');
	}
	return out_str;
}

std::string debugger::Debugger::print_flags() {
	// build the string
	std::string out_str = "flags:\n  [ ";
	std::string end_str = "";

	// append the zero-flag
	if (_context->flag_info.zero_value == 0)
		out_str.append("ZF ");

	// append the sign-flag
	if ((_context->flag_info.sign_value >> _context->flag_info.sign_size) == 1)
		out_str.append("SF ");

	// append the parity-flag
	uint8_t temp = (_context->flag_info.parity_value & 0x0fu) ^(_context->flag_info.parity_value >> 4u);
	temp = (temp & 0x03u) ^ (temp >> 2u);
	temp = (temp & 0x01u) ^ (temp >> 1u);
	if (temp == 0)
		out_str.append("PF ");

	// append the carry-flag
	if (_context->flag_info.carry_operation == static_cast<uint16_t>(codegen::jump_table::Entry::unsupported_carry) * 4) {
		end_str.append("CF: ").append(reinterpret_cast<const char*>(_context->flag_info.carry_pointer));
	} else if (_context->flag_info.carry_operation == static_cast<uint16_t>(codegen::jump_table::Entry::high_level_carry) * 4) {
		if (reinterpret_cast<codegen::Instruction::c_callback_t>(_context->flag_info.carry_pointer)(_context) != 0)
			out_str.append("CF ");
		else {
			dispatcher::ExecutionContext::Context temp_context;
			if (evaluate_carry(_context, &temp_context) != 0)
				out_str.append("CF ");
		}
	}

	// append the overflow-flag
	if (_context->flag_info.overflow_operation == static_cast<uint16_t>(codegen::jump_table::Entry::unsupported_overflow) * 4) {
		if (!end_str.empty())
			end_str.append("; ");
		end_str.append("OF: ").append(reinterpret_cast<const char*>(_context->flag_info.overflow_pointer));
	} else if (_context->flag_info.carry_operation ==
			   static_cast<uint16_t>(codegen::jump_table::Entry::high_level_overflow) * 4) {
		if (reinterpret_cast<codegen::Instruction::c_callback_t>(_context->flag_info.overflow_pointer)(_context) != 0)
			out_str.append("OF ");
		else {
			dispatcher::ExecutionContext::Context temp_context;
			if (evaluate_overflow(_context, &temp_context) != 0)
				out_str.append("OF ");
		}
	}
	out_str.append("] ").append(end_str).push_back('\n');
	return out_str;
}

std::string debugger::Debugger::print_break_points() {
	// iterate through the break-points and print them
	std::string out_string = "break-points:\n";
	for (size_t i = 0; i < static_cast<size_t>(_bp_count - ((_state & DebugState::temp_break) ? 1 : 0)); i++) {
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

std::string debugger::Debugger::print_stack(uintptr_t address, uint16_t limit) {
	std::string out_str = "stack:\n";

	// clip the address
	address = address ^ (address & 0x07);
	address += (limit / 2) * 8;
	if (address > _stack_high)
		address = _stack_high;
	else if (address < _stack_low + (limit - 1) * 8)
		address = _stack_low + (limit - 1) * 8;

	// iterate through the stack and print it
	for (size_t i = 0; i < limit; i++) {
		// add the address
		out_str.append("  [");
		out_str.append(print_number(address, true));
		out_str.append("] = ");

		// add the value
		if (address == _stack_high)
			out_str.append(" above the stack  ");
		else if (address == _stack_low)
			out_str.append("beneath the stack ");
		else
			out_str.append(print_number(reinterpret_cast<uint64_t*>(address)[0], true));
		out_str.append(" <- ");

		// add the offset to rsp
		std::string temp_str = "[";
		if (address == _context->guest.map.rsp)
			temp_str.append("rsp");
		else if (address > _context->guest.map.rsp) {
			temp_str.append("rsp+");
			temp_str.append(print_number(address - _context->guest.map.rsp, false));
		} else {
			temp_str.append("rsp-");
			temp_str.append(print_number(_context->guest.map.rsp - address, false));
		}
		temp_str.push_back(']');
		if (temp_str.size() < 12)
			temp_str.insert(temp_str.size(), 12 - temp_str.size(), ' ');

		// add the offset to tbp
		if (address == _context->guest.map.rbp)
			temp_str.append("[rbp");
		else if (address > _context->guest.map.rbp) {
			temp_str.append("[rbp+");
			temp_str.append(print_number(address - _context->guest.map.rbp, false));
		} else {
			temp_str.append("[rbp-");
			temp_str.append(print_number(_context->guest.map.rbp - address, false));
		}
		temp_str.push_back(']');
		out_str.append(temp_str);
		out_str.push_back('\n');

		// update the address
		address -= 8;
	}
	return out_str;
}

std::string debugger::Debugger::print_blocks() {
	std::string out_str = "basic blocks:\n";

	// iterate through the blocks and write them to the string
	for (size_t i = 0; i < _blocks.size(); i++) {
		// check if a break-point lies within the block
		bool contains_bp = false;
		for (size_t j = 0; j < static_cast<size_t>(_bp_count - ((_state & DebugState::temp_break) ? 1 : 0)); j++) {
			if (_bp_x86_array[j] >= _blocks[i].entry->x86_start && _bp_x86_array[j] < _blocks[i].entry->x86_end) {
				contains_bp = true;
				break;
			}
		}

		// build the initial string
		std::string temp_str = (_current == &_blocks[i] ? " -> " : "    ");
		temp_str.push_back(contains_bp ? '*' : ' ');

		// write the block-index
		temp_str.append(print_number(i, false, 3));
		temp_str.append(". x86: [");
		temp_str.append(print_number(_blocks[i].entry->x86_start, true));
		temp_str.append(" - ");
		temp_str.append(print_number(_blocks[i].entry->x86_end, true));
		temp_str.append("]\n        riscv: [");
		temp_str.append(print_number(_blocks[i].entry->riscv_start, true));
		temp_str.append(" - ");
		temp_str.append(print_number(_blocks[i].riscv_end, true));
		temp_str.append("] [count: ");
		temp_str.append(print_number(_blocks[i].entry->instruction_count, false));
		out_str.append(temp_str);
		out_str.append("]\n");
	}
	return out_str;
}

std::string debugger::Debugger::print_memory(uintptr_t address, uint32_t count) {
	std::string out_str = "memory:\n";

	// iterate through the memory and print it
	for (size_t i = 0; i < count; i++) {
		// add the address
		out_str.append("  [");
		out_str.append(print_number(address, true));
		out_str.append("] = ");

		// add the value
		out_str.append(print_number(reinterpret_cast<uint64_t*>(address)[0], true));
		out_str.push_back('\n');

		// update the address
		address += 8;
	}
	return out_str;
}