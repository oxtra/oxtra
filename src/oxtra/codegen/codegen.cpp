#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;


CodeGenerator::CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf)
		: _args{args}, _elf{elf}, _codestore{args, elf} {}

host_addr_t CodeGenerator::translate(guest_addr_t addr) {
	/*
	 * max block size = min(next_codeblock.start, instruction offset limit)
	 *
	 * loop over instructions and decode
	 * - big switch for each instruction type
	 * - translate instruction
	 * - add risc-v instructions into code store
	 *
	 * add jump to dispatcher::host_enter
	 * return address to translated code
	 */
	auto& codeblock = _codestore.create_block();
	auto current_address = reinterpret_cast<const uint8_t*>(_elf.resolve_vaddr(addr));
	bool end_of_block;

	do {
		auto x86_instruction = Instruction{};
		if (decode(current_address, _elf.get_size(addr), DecodeMode::decode_64, addr, x86_instruction) <= 0)
			throw std::runtime_error("Failed to decode the instruction");

		current_address += x86_instruction.get_size();
		addr += x86_instruction.get_size();

		size_t num_instructions = 0;
		riscv_instruction_t riscv_instructions[max_riscv_instructions];

		end_of_block = translate_instruction(x86_instruction, riscv_instructions, num_instructions);

		// add tracing-information
		if constexpr (SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE) {
			char formatted_string[512];
			fadec::format(x86_instruction, formatted_string, sizeof(formatted_string));

			SPDLOG_TRACE("decoded {}", formatted_string);

			for (size_t i = 0; i < num_instructions; i++)
				SPDLOG_TRACE(" - instruction[{}] = {}", i, decoding::parse_riscv(riscv_instructions[i]));
		}

		_codestore.add_instruction(codeblock, x86_instruction, riscv_instructions, num_instructions);
	} while (!end_of_block);

	//add dynamic tracing-information for the basic-block
	spdlog::trace("Basicblock translated: x86: [0x{0:x} - 0x{1:x}] riscv: 0x{2:x}", codeblock.x86_start,
				  codeblock.x86_end, codeblock.riscv_start);

	return codeblock.riscv_start;
}

void CodeGenerator::apply_operation(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count,
									void(* callback)(const fadec::Instruction&, encoding::RiscVRegister,
													 encoding::RiscVRegister, utils::riscv_instruction_t*, size_t&)) {
	// extract the source-operand
	RiscVRegister source_register = source_temp_register;
	if (inst.get_operand(1).get_type() == OperandType::reg &&
		inst.get_operand(1).get_register_type() != RegisterType::gph)
		source_register = register_mapping[static_cast<uint16_t>(inst.get_operand(1).get_register())];
	else
		translate_operand(inst, 1, source_register, riscv, count);

	// extract the register for the destination-value
	RiscVRegister dest_register = dest_temp_register;
	RiscVRegister address = RiscVRegister::zero;
	if (inst.get_operand(0).get_type() == OperandType::reg && inst.get_operand(0).get_size() == 8)
		dest_register = register_mapping[static_cast<uint16_t>(inst.get_operand(0).get_register())];
	else
		address = translate_operand(inst, 0, dest_register, riscv, count);

	// call the callback to apply the changes
	callback(inst, dest_register, source_register, riscv, count);

	// write the value back to the destination
	translate_destination(inst, dest_register, address, riscv, count);
}

encoding::RiscVRegister
CodeGenerator::translate_operand(const fadec::Instruction& inst, size_t index, encoding::RiscVRegister reg,
								 utils::riscv_instruction_t* riscv, size_t& count) {
	// extract the operand
	auto& operand = inst.get_operand(index);

	// load the source-operand into the temporary-register
	if (operand.get_type() == OperandType::reg) {
		/* read the value from the register (read the whole register
		 * (unless HBYTE is required), and just cut the rest when writing the register */
		if (operand.get_register_type() == RegisterType::gph) {
			if (operand.get_register() == Register::ah)
				get_from_register(reg, register_mapping[static_cast<uint16_t>(Register::rax)],
								  RegisterAccess::HBYTE, riscv, count);
			else if (operand.get_register() == Register::bh)
				get_from_register(reg, register_mapping[static_cast<uint16_t>(Register::rbx)],
								  RegisterAccess::HBYTE, riscv, count);
			else if (operand.get_register() == Register::ch)
				get_from_register(reg, register_mapping[static_cast<uint16_t>(Register::rcx)],
								  RegisterAccess::HBYTE, riscv, count);
			else
				get_from_register(reg, register_mapping[static_cast<uint16_t>(Register::rdx)],
								  RegisterAccess::HBYTE, riscv, count);
		} else
			get_from_register(reg, register_mapping[static_cast<uint16_t>(operand.get_register())],
							  RegisterAccess::QWORD, riscv, count);
	} else if (operand.get_type() == OperandType::imm)
		load_unsigned_immediate(inst.get_immediate(), reg, riscv, count);
	else {
		// read the value from memory
		translate_memory_operand(inst, 1, address_temp_register, riscv, count);
		switch (operand.get_size()) {
			case 8:
				riscv[count++] = encoding::LD(address_temp_register, reg, 0);
				break;
			case 4:
				riscv[count++] = encoding::LW(address_temp_register, reg, 0);
				break;
			case 2:
				riscv[count++] = encoding::LH(address_temp_register, reg, 0);
				break;
			case 1:
				riscv[count++] = encoding::LB(address_temp_register, reg, 0);
				break;
		}
		return address_temp_register;
	}
	return encoding::RiscVRegister::zero;
}

void CodeGenerator::translate_destination(const fadec::Instruction& inst, encoding::RiscVRegister reg,
										  encoding::RiscVRegister address, utils::riscv_instruction_t* riscv,
										  size_t& count) {
	auto& operand = inst.get_operand(0);

	// check if the destination is a register
	if (operand.get_type() == OperandType::reg) {
		RiscVRegister temp_reg = register_mapping[static_cast<uint16_t>(operand.get_register())];
		switch (operand.get_size()) {
			case 8:
				if (temp_reg != reg)
					move_to_register(temp_reg, reg, RegisterAccess::QWORD, riscv, count);
				break;
			case 4:
				move_to_register(temp_reg, reg, RegisterAccess::DWORD, riscv, count);
				break;
			case 2:
				move_to_register(temp_reg, reg, RegisterAccess::WORD, riscv, count);
				break;
			case 1:
				if (operand.get_register_type() == RegisterType::gph) {
					if (operand.get_register() == Register::ah)
						move_to_register(register_mapping[static_cast<uint16_t>(Register::rax)], reg,
										 RegisterAccess::HBYTE, riscv, count);
					else if (operand.get_register() == Register::bh)
						move_to_register(register_mapping[static_cast<uint16_t>(Register::rbx)], reg,
										 RegisterAccess::HBYTE, riscv, count);
					else if (operand.get_register() == Register::ch)
						move_to_register(register_mapping[static_cast<uint16_t>(Register::rcx)], reg,
										 RegisterAccess::HBYTE, riscv, count);
					else
						move_to_register(register_mapping[static_cast<uint16_t>(Register::rdx)], reg,
										 RegisterAccess::HBYTE, riscv, count);
				} else
					move_to_register(temp_reg, reg, RegisterAccess::LBYTE, riscv, count);
				break;
		}
		return;
	}

	// translate the memory-address and write the value to it
	if (address == encoding::RiscVRegister::zero) {
		translate_memory_operand(inst, 0, temp1_register, riscv, count);
		address = temp1_register;
	}
	switch (operand.get_size()) {
		case 8:
			riscv[count++] = encoding::SD(address, reg, 0);
			break;
		case 4:
			riscv[count++] = encoding::SW(address, reg, 0);
			break;
		case 2:
			riscv[count++] = encoding::SH(address, reg, 0);
			break;
		case 1:
			riscv[count++] = encoding::SB(address, reg, 0);
			break;
	}
}

void CodeGenerator::translate_memory_operand(const Instruction& inst, size_t index, RiscVRegister reg,
											 riscv_instruction_t* riscv, size_t& count) {
	if (inst.get_address_size() < 4)
		throw std::runtime_error("invalid addressing-size");
	const auto& operand = inst.get_operand(index);

	// add the scale & index
	if (inst.get_index_register() != fadec::Register::none) {
		if (inst.get_address_size() == 8)
			get_from_register(reg, register_mapping[static_cast<uint16_t>( inst.get_index_register())],
							  RegisterAccess::QWORD, riscv, count);
		else
			get_from_register(reg, register_mapping[static_cast<uint16_t>( inst.get_index_register())],
							  RegisterAccess::DWORD, riscv, count);
		riscv[count++] = encoding::SLLI(reg, reg, inst.get_index_scale());
	} else
		load_unsigned_immediate(0, reg, riscv, count);

	// add the base-register
	if (operand.get_register() != fadec::Register::none) {
		if (inst.get_address_size() == 8)
			riscv[count++] = encoding::ADD(reg, reg, register_mapping[static_cast<uint16_t>( operand.get_register())]);
		else {
			get_from_register(memory_temp_register, register_mapping[static_cast<uint16_t>(operand.get_register())],
							  RegisterAccess::DWORD, riscv, count);
			riscv[count++] = encoding::ADD(reg, reg, memory_temp_register);
		}
	}

	// add the displacement
	if (inst.get_displacement() > 0 || _elf.get_address_delta() > 0) {
		uintptr_t displacement = _elf.get_address_delta() + inst.get_displacement();
		// less or equal than 12 bits
		if (displacement < 0x800) {
			riscv[count++] = encoding::ADDI(reg, reg, static_cast<uint16_t>(displacement));
		} else {
			load_unsigned_immediate(displacement, memory_temp_register, riscv, count);
			riscv[count++] = encoding::ADD(reg, reg, memory_temp_register);
		}
	}
}

void CodeGenerator::move_to_register(RiscVRegister dest, RiscVRegister src, RegisterAccess access,
									 riscv_instruction_t* riscv, size_t& count) {
	switch (access) {
		case RegisterAccess::QWORD:
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case RegisterAccess::DWORD:
			// clear the lower bits of the destination-register by shifting
			riscv[count++] = encoding::SRLI(dest, dest, 32);
			riscv[count++] = encoding::SLLI(dest, dest, 32);

			// copy the source-register and clear the upper bits by shifting
			riscv[count++] = encoding::SLLI(read_temp_register, src, 32);
			riscv[count++] = encoding::SRLI(read_temp_register, read_temp_register, 32);

			// combine the registers
			riscv[count++] = encoding::OR(dest, dest, read_temp_register);
			return;
		case RegisterAccess::WORD:
			// clear the lower bits of the destination-register by shifting
			riscv[count++] = encoding::SRLI(dest, dest, 16);
			riscv[count++] = encoding::SLLI(dest, dest, 16);

			// copy the source-register and clear the upper bits by shifting
			riscv[count++] = encoding::SLLI(read_temp_register, src, 48);
			riscv[count++] = encoding::SRLI(read_temp_register, read_temp_register, 48);

			// combine the registers
			riscv[count++] = encoding::OR(dest, dest, read_temp_register);
			return;
		case RegisterAccess::LBYTE:
			// clear the lower bits of the destination-register
			riscv[count++] = encoding::ANDI(read_temp_register, dest, 0xff);
			riscv[count++] = encoding::XOR(dest, dest, read_temp_register);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::ANDI(read_temp_register, src, 0xff);
			riscv[count++] = encoding::OR(dest, dest, read_temp_register);
			return;
		case RegisterAccess::HBYTE:
			// load the and-mask
			load_unsigned_immediate(0xff00, read_temp_register, riscv, count);

			// clear the lower bits of the destination-register
			riscv[count++] = encoding::AND(read_temp_register, read_temp_register, dest);
			riscv[count++] = encoding::XOR(dest, dest, read_temp_register);

			// extract the lower bits of the source-register and merge the registers
			riscv[count++] = encoding::ANDI(read_temp_register, src, 0xff);
			riscv[count++] = encoding::SLLI(read_temp_register, read_temp_register, 8);
			riscv[count++] = encoding::OR(dest, dest, read_temp_register);
			return;
	}
}

void CodeGenerator::get_from_register(RiscVRegister dest, RiscVRegister src, RegisterAccess access,
									  riscv_instruction_t* riscv, size_t& count) {
	switch (access) {
		case RegisterAccess::QWORD:
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case RegisterAccess::DWORD:
			// load copy the register and shift-clear it
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			riscv[count++] = encoding::SLLI(dest, dest, 32);
			riscv[count++] = encoding::SRLI(dest, dest, 32);
			return;
		case RegisterAccess::WORD:
			// load copy the register and shift-clear it
			riscv[count++] = encoding::ADD(dest, src, RiscVRegister::zero);
			riscv[count++] = encoding::SLLI(dest, dest, 48);
			riscv[count++] = encoding::SRLI(dest, dest, 48);
			return;
		case RegisterAccess::LBYTE:
			riscv[count++] = encoding::ANDI(dest, src, 0xff);
			return;
		case RegisterAccess::HBYTE:
			riscv[count++] = encoding::SRLI(dest, src, 8);
			riscv[count++] = encoding::ANDI(dest, dest, 0xff);
			return;
	}
}

void CodeGenerator::load_12bit_immediate(uint16_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = encoding::ADDI(dest, RiscVRegister::zero, static_cast<uint16_t>(imm) & 0x0FFFu);
}

void CodeGenerator::load_32bit_immediate(uint32_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	auto upper_immediate = static_cast<uint32_t>(imm >> 12u);
	const auto lower_immediate = static_cast<uint16_t>(imm & 0x0FFFu);

	// adding the lower bits is sign extended, so if the lower bits are signed we have to increase the upper immediate
	if (imm & 0x800u)
		upper_immediate++;

	riscv[count++] = encoding::LUI(dest, upper_immediate);

	if (lower_immediate != 0x0000) {
		riscv[count++] = encoding::ADDI(dest, dest, lower_immediate);
	}
}

void CodeGenerator::load_64bit_immediate(uint64_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	const uint32_t high_bits = (imm >> 32u) & 0xFFFFFFFF;
	const uint32_t low_bits = imm & 0xFFFFFFFF;
	constexpr size_t immediate_count = 4;
	uint32_t immediates[immediate_count] = {low_bits & 0x000000FFu, (low_bits & 0x000FFF00u) >> 8u,
											(low_bits & 0xFFF00000u) >> 20u, high_bits};

	for (size_t i = immediate_count - 2; i >= 1; i--) {
		if (immediates[i] & 0x800u)
			immediates[i + 1]++;
	}
	// load upper 32bit into destination
	load_32bit_immediate(immediates[immediate_count - 1], dest, riscv, count);

	for (int8_t i = immediate_count - 2; i >= 0; i--) {
		// add the next 12bit (8 bit for the last one)
		riscv[count++] = encoding::SLLI(dest, dest, (i == 0) ? 8 : 12);
		if (immediates[i] != 0) {
			riscv[count++] = encoding::ADDI(dest, dest, immediates[i]);
		}
	}
}

void
CodeGenerator::load_signed_immediate(uintptr_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	uintptr_t short_value = (imm & 0xFFFu);
	if (short_value & 0x800u) {
		short_value |= 0xFFFFFFFFFFFFF000;
	}

	uintptr_t word_value = imm & 0xFFFFFFFFu;
	if (word_value & 0x80000000) {
		word_value |= 0xFFFFFFFF00000000;
	}

	if (imm == short_value) {    // 12 bit can be directly encoded
		load_12bit_immediate(static_cast<uint16_t>(imm), dest, riscv, count);
	} else if (imm == word_value) {    //32 bit have to be manually specified
		load_32bit_immediate(static_cast<uint32_t>(imm), dest, riscv, count);
	} else { // 64 bit also have to be manually specified
		load_64bit_immediate(static_cast<uint64_t>(imm), dest, riscv, count);
	}
}

void
CodeGenerator::load_unsigned_immediate(uintptr_t imm, RiscVRegister dest, riscv_instruction_t* riscv, size_t& count) {
	uint8_t immediate_type = 2; // 0 means 12bit, 1 means 32 bit, >= 2 means 64 bit

	// if it is a signed immediate we have to use the bigger type to ensure that it is padded with zeros.
	// otherwise add methods may not work correctly
	if (imm < 0x1000) {    // 12 bits can be directly encoded
		immediate_type = imm & 0x0800u ? 1 : 0; // but only if it is a positive 12 bit (sign bit not set)
	} else if (imm < 0x100000000) {
		immediate_type = imm & 0x80000000 ? 2 : 1; // same with 32 bit
	}

	if (immediate_type == 0) {
		load_12bit_immediate(static_cast<uint16_t>(imm), dest, riscv, count);
	} else if (immediate_type == 1) {
		load_32bit_immediate(static_cast<uint32_t>(imm), dest, riscv, count);
	} else {
		load_64bit_immediate(static_cast<uint64_t>(imm), dest, riscv, count);
	}
}