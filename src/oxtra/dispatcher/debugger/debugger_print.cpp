#include "debugger.h"
#include <string>
#include <oxtra/dispatcher/execution_context.h>

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
		if(dec_digits > 0)
			str.insert(0, dec_digits, dec_pad);
		str.insert(0, 1, '-');
		return str;
	}

	// convert the number to a string
	if(nbr == 0) {
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
	if(dec_digits > 0)
		str.insert(0, dec_digits, dec_pad);
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

std::string debugger::Debugger::print_assembly(utils::guest_addr_t guest, BlockEntry* entry, uint16_t limit) {
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
		if (!decode_failed) {
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
		if (line_buffer.size() < 50)
			line_buffer.insert(line_buffer.size(), 50 - line_buffer.size(), ' ');

		// update the x86-data
		if (!decode_failed) {
			x86_address += inst.get_size();
			index++;
		}

		// check any riscv-code exists
		if (i < riscv_count && i + 1 == limit)
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

std::string debugger::Debugger::print_stack(uintptr_t address, dispatcher::ExecutionContext* context, uint16_t limit) {
	std::string out_str = "stack:\n";

	// clip the address
	address = address ^ (address & 0x07);
	if(address > _stack_high)
		address = _stack_high;
	else if(address - (limit - 1) * 8 <= _stack_low)
		address = _stack_low + (limit - 1) * 8;

	// iterate through the stack and print it
	for (size_t i = 0; i < limit; i++){
		// add the address
		out_str.append("  [");
		out_str.append(print_number(address, true));
		out_str.append("] = ");

		// add the value
		if(address == _stack_high)
			out_str.append("  above the stack  ");
		else if(address == _stack_low + (limit - 1) * 8)
			out_str.append(" beneath the stack ");
		else
			out_str.append(print_number(reinterpret_cast<uint64_t*>(address)[0], true));
		out_str.append(" <- ");

		// add the offset to rsp
		std::string temp_str = "[";
		if(address == context->guest.map.rsp)
			temp_str.append("rsp");
		else if(address > context->guest.map.rsp){
			temp_str.append("rsp-");
			temp_str.append(print_number(address - context->guest.map.rsp, false));
		}
		else{
			temp_str.append("rsp+");
			temp_str.append(print_number(context->guest.map.rsp - address, false));
		}
		temp_str.push_back(']');
		if(temp_str.size() < 12)
			temp_str.insert(temp_str.size(), 12 - temp_str.size(), ' ');

		// add the offset to tbp
		if(address == context->guest.map.rbp)
			temp_str.append("[rbp");
		else if(address > context->guest.map.rbp){
			temp_str.append("[rbp-");
			temp_str.append(print_number(address - context->guest.map.rbp, false));
		}
		else{
			temp_str.append("[rbp+");
			temp_str.append(print_number(context->guest.map.rbp - address, false));
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
		for (size_t j = 0; j < _bp_count; j++) {
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