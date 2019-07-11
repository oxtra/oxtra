#include "oxtra/codegen/decoding/decoding.h"

using namespace std;
using namespace utils;
using namespace encoding;

enum class RiscVOpcode : uint8_t {
	LUI, AUIPC,
	LB, LH, LW, LBU, LHU, LWU, LD, ADDI, SLTI, SLTIU, XORI, ORI, ANDI,
	FLW, FLD, ADDIW, SLLIW, SRLIW, SRAIW, SLLI, SRLI, SRAI, SB, SH, SW, SD,

	MUL, MULH, MULHSU, MULHU, DIV, DIVU, REM, REMU, ADD, SUB, SLL, SLT,
	SLTU, XOR, SRL, SRA, OR, AND, MULW, DIVW, DIVUW, REMW, REMUW, ADDW,
	SUBW, SLLW, SRLW, SRAW,

	JAL, JALR, BEQ, BNE, BLT, BGE, BLTU, BGEU
};

constexpr const char* register_string[32] = {
		//"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1",
		//"a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
		//"s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
		//"t3", "t4", "t5", "t6"

		"zero", "Vra", "rsp", "Vgp", "Vtp", "Vt0", "Vt1", "Vt2", "rbp",
		"flags", "rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9",
		"r10", "r11", "r12", "r13", "r14", "r15", "reroute_static", "reroute_dynamic",
		"Vs10", "context", "inline_destination", "Vt4", "Vt5", "Vt6"
};
constexpr const char* opcode_string[128] = {
		"lui", "auipc", "lb", "lh", "lw", "lbu", "lhu", "lwu", "ld",
		"addi", "slti", "sltiu", "xori", "ori", "andi", "flw", "fld",
		"addiw", "slliw", "srliw", "sraiw", "slli", "srli", "srai", "sb",
		"sh", "sw", "sd", "mul", "mulh", "mulhsu", "mulhu", "div", "divu",
		"rem", "remu", "add", "sub", "sll", "slt", "sltu", "xor", "srl",
		"sra", "or", "and", "mulw", "divw", "divuw", "remw", "remuw",
		"addw", "subw", "sllw", "srlw", "sraw", "jal", "jalr", "beq",
		"bne", "blt", "bge", "bltu", "bgeu"
};
constexpr const char* error_string = "unknown instruction";

// split off a number of bits at a given offset (default size to 5 for the registers)
uint32_t split_off(riscv_instruction_t instruction, uint8_t offset, uint8_t size = 5) {
	return (instruction >> offset) & (0xFFFFFFFFu >> (32u - size));
}

// add numbers
void parse_number(stringstream& sstr, uint32_t nbr, bool sign) {
	sstr << (sign ? " -" : " ");
	sstr << "0x" << hex << nbr << (sign ? "(-" : "(") << dec << nbr << ")";
}

// create the stream
stringstream initialize_string(RiscVOpcode opcode){
	// creaate the sstream with the opcode
	stringstream sstr;
	sstr << opcode_string[static_cast<uint8_t>(opcode)];
	return sstr;
}

// register-register instructions
string parse_rtype(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// parse the two source-registers
	sstr << " " << register_string[split_off(instruction, 15)];
	sstr << ", " << register_string[split_off(instruction, 20)];

	// parse the destination-register
	sstr << " -> " << register_string[split_off(instruction, 7)];
	return sstr.str();
}

// register-register instructions (with shifts)
string parse_shift(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// parse the source-register
	sstr << " " << register_string[split_off(instruction, 15)];

	// parse the shift
	sstr << ",";
	parse_number(sstr, split_off(instruction, 20, 6), false);

	// parse the destination-register
	sstr << " -> " << register_string[split_off(instruction, 7)];
	return sstr.str();
}

//immediate-register instructions
string parse_itype(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// parse the source-register
	sstr << " " << register_string[split_off(instruction, 15)] << ",";

	// extract the immediate and parse it
	uint16_t immediate = split_off(instruction, 20, 12);
	if (immediate & 0x0800u) {
		immediate |= 0xf000u;
		immediate = (~immediate) + 1;
		parse_number(sstr, immediate, true);
	} else
		parse_number(sstr, immediate, false);

	// parse the destination-register
	sstr << " -> " << register_string[split_off(instruction, 7)];
	return sstr.str();
}

//upper-register-immediate instructions
string parse_utype(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// parse the source-immediate
	parse_number(sstr, split_off(instruction, 12, 20), false);

	// parse the destination-register
	sstr << " -> " << register_string[split_off(instruction, 7)];
	return sstr.str();
}

//load instructions
string parse_load(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// prase the base-register
	sstr << " [" << register_string[split_off(instruction, 15)];

	// extract the offset
	uint16_t offset = split_off(instruction, 20, 12);
	if (offset > 0) {
		sstr << " +";
		// sign-extend the offset
		if (offset & 0x0800u) {
			offset |= 0xf000u;
			offset = (~offset) + 1;
			parse_number(sstr, offset, true);
		} else
			parse_number(sstr, offset, false);
	}
	sstr << "] -> ";

	// parse the destination-register
	sstr << register_string[split_off(instruction, 7)];
	return sstr.str();
}

//store instructions
string parse_store(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// parse the source-register
	sstr << " " << register_string[split_off(instruction, 20)];

	// prase the base-register
	sstr << " -> [" << register_string[split_off(instruction, 15)];

	// extract the offset
	uint16_t offset = split_off(instruction, 7) | (split_off(instruction, 25, 7) << 5u);
	if (offset > 0) {
		sstr << " +";
		// sign-extend the offset
		if (offset & 0x0800u) {
			offset |= 0xf000u;
			offset = (~offset) + 1;
			parse_number(sstr, offset, true);
		} else
			parse_number(sstr, offset, false);
	}
	sstr << "]";
	return sstr.str();
}

//jump-type instructions
string parse_jtype(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// extract the offset
	uint32_t offset = (split_off(instruction, 21, 10) << 1u) | (split_off(instruction, 20, 1) << 11u) |
					  (split_off(instruction, 12, 8) << 12u) | (split_off(instruction, 31, 1) << 20u);

	// parse the offset
	sstr << " $[pc";
	if (offset > 0) {
		sstr << " +";
		// sign-extend the offset
		if (offset & 0x100000u) {
			offset |= 0xffe00000u;
			offset = (~offset) + 1;
			parse_number(sstr, offset, true);
		} else
			parse_number(sstr, offset, false);
	}
	sstr << "] @ ";

	// parse the destination-register
	sstr << register_string[split_off(instruction, 7)];
	return sstr.str();
}

//relative-jump-type instructions
string parse_relative(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// parse the base-register
	sstr << " $[" << register_string[split_off(instruction, 15)];

	// extract the offset
	uint16_t offset = split_off(instruction, 20, 12);

	// parse the offset
	if (offset > 0) {
		sstr << " +";
		// sign-extend the offset
		if (offset & 0x0800u) {
			offset |= 0xf000u;
			offset = (~offset) + 1;
			parse_number(sstr, offset, true);
		} else
			parse_number(sstr, offset, false);
	}
	sstr << "] @ ";

	// parse the destination-register
	sstr << register_string[split_off(instruction, 7)];
	return sstr.str();
}

//branch-type instructions
string parse_btype(RiscVOpcode opcode, riscv_instruction_t instruction) {
	stringstream sstr = initialize_string(opcode);

	// parse the two source-registers
	sstr << " " << register_string[split_off(instruction, 15)];
	sstr << ", " << register_string[split_off(instruction, 20)] << " ? ";

	// extract the offset
	uint16_t offset = (split_off(instruction, 8, 4) << 1u) | (split_off(instruction, 25, 6) << 5u) |
					  (split_off(instruction, 7, 1) << 11u) | (split_off(instruction, 31, 1) << 12u);

	// parse the offset
	sstr << "$[pc";
	if (offset > 0) {
		sstr << " +";
		// sign-extend the offset
		if (offset & 0x1000u) {
			offset |= 0xe000u;
			offset = (~offset) + 1;
			parse_number(sstr, offset, true);
		} else
			parse_number(sstr, offset, false);
	}
	sstr << "]";

	return sstr.str();
}

//implementation of the parsing-function
string decoding::parse_riscv(riscv_instruction_t instruction) {
	// split off the opcode and the functional-codes
	uint8_t opcode = split_off(instruction, 0, 7);
	uint8_t func3 = split_off(instruction, 12, 3);
	uint8_t func7 = split_off(instruction, 25, 7);

	//handle the opcode
	switch (opcode) {
		/* following are all of the jump & branch-instructions */
		case 0x67:
			if (func3 == 0x00)
				return parse_relative(RiscVOpcode::JALR, instruction);
			return error_string;
		case 0x6f:
			return parse_jtype(RiscVOpcode::JAL, instruction);
		case 0x63:
			switch (func3) {
				case 0x00:
					return parse_btype(RiscVOpcode::BEQ, instruction);
				case 0x01:
					return parse_btype(RiscVOpcode::BNE, instruction);
				case 0x04:
					return parse_btype(RiscVOpcode::BLT, instruction);
				case 0x05:
					return parse_btype(RiscVOpcode::BGE, instruction);
				case 0x06:
					return parse_btype(RiscVOpcode::BLTU, instruction);
				case 0x07:
					return parse_btype(RiscVOpcode::BGEU, instruction);
			}
			return error_string;

			/* following are all of the utype-instructions */
		case 0x37:
			return parse_utype(RiscVOpcode::LUI, instruction);
		case 0x17:
			return parse_utype(RiscVOpcode::AUIPC, instruction);

			/* following are all of the store - instructions */
		case 0x23:
			switch (func3) {
				case 0x00:
					return parse_store(RiscVOpcode::SB, instruction);
				case 0x01:
					return parse_store(RiscVOpcode::SH, instruction);
				case 0x02:
					return parse_store(RiscVOpcode::SW, instruction);
				case 0x03:
					return parse_store(RiscVOpcode::SD, instruction);
			}
			return error_string;

			/* following are all of the itype & load - instructions */
		case 0x03:
			switch (func3) {
				case 0x00:
					return parse_load(RiscVOpcode::LB, instruction);
				case 0x01:
					return parse_load(RiscVOpcode::LH, instruction);
				case 0x02:
					return parse_load(RiscVOpcode::LW, instruction);
				case 0x03:
					return parse_load(RiscVOpcode::LD, instruction);
				case 0x04:
					return parse_load(RiscVOpcode::LBU, instruction);
				case 0x05:
					return parse_load(RiscVOpcode::LHU, instruction);
				case 0x06:
					return parse_load(RiscVOpcode::LWU, instruction);
			}
			return error_string;
		case 0x07:
			if (func3 == 0x02)
				return parse_load(RiscVOpcode::FLW, instruction);
			if (func3 == 0x03)
				return parse_load(RiscVOpcode::FLD, instruction);
			return error_string;
		case 0x13:
			switch (func3) {
				case 0x00:
					return parse_itype(RiscVOpcode::ADDI, instruction);
				case 0x01:
					if (func7 == 0x00 || func7 == 0x01)
						return parse_shift(RiscVOpcode::SLLI, instruction);
					return error_string;
				case 0x02:
					return parse_itype(RiscVOpcode::SLTI, instruction);
				case 0x03:
					return parse_itype(RiscVOpcode::SLTIU, instruction);
				case 0x04:
					return parse_itype(RiscVOpcode::XORI, instruction);
				case 0x05:
					if (func7 == 0x00 || func7 == 0x01)
						return parse_shift(RiscVOpcode::SRLI, instruction);
					else if (func7 == 0x20 || func7 == 0x21)
						return parse_shift(RiscVOpcode::SRAI, instruction);
					return error_string;
				case 0x06:
					return parse_itype(RiscVOpcode::ORI, instruction);
				case 0x07:
					return parse_itype(RiscVOpcode::ANDI, instruction);
			}
			return error_string;
		case 0x1b:
			switch (func3) {
				case 0x00:
					return parse_itype(RiscVOpcode::ADDIW, instruction);
				case 0x01:
					if (func7 == 0x00)
						return parse_shift(RiscVOpcode::SLLIW, instruction);
					return error_string;
				case 0x05:
					if (func7 == 0x00)
						return parse_shift(RiscVOpcode::SRLIW, instruction);
					else if (func7 == 0x20)
						return parse_shift(RiscVOpcode::SRAIW, instruction);
					return error_string;
			}
			return error_string;

			/* the following are all of the rtype-instructions */
		case 0x33:
			if (func7 == 0x01) {
				switch (func3) {
					case 0x00:
						return parse_rtype(RiscVOpcode::MUL, instruction);
					case 0x01:
						return parse_rtype(RiscVOpcode::MULH, instruction);
					case 0x02:
						return parse_rtype(RiscVOpcode::MULHSU, instruction);
					case 0x03:
						return parse_rtype(RiscVOpcode::MULHU, instruction);
					case 0x04:
						return parse_rtype(RiscVOpcode::DIV, instruction);
					case 0x05:
						return parse_rtype(RiscVOpcode::DIVU, instruction);
					case 0x06:
						return parse_rtype(RiscVOpcode::REM, instruction);
					case 0x07:
						return parse_rtype(RiscVOpcode::REMU, instruction);
				}
				return error_string;
			}
			switch (func3) {
				case 0x00:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::ADD, instruction);
					else if (func7 == 0x20)
						return parse_rtype(RiscVOpcode::SUB, instruction);
					return error_string;
				case 0x01:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::SLL, instruction);
					return error_string;
				case 0x02:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::SLT, instruction);
					return error_string;
				case 0x03:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::SLTU, instruction);
					return error_string;
				case 0x04:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::XOR, instruction);
					return error_string;
				case 0x05:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::SRL, instruction);
					else if (func7 == 0x20)
						return parse_rtype(RiscVOpcode::SRA, instruction);
					return error_string;
				case 0x06:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::OR, instruction);
					return error_string;
				case 0x07:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::AND, instruction);
					return error_string;
			}
			return error_string;
		case 0x3b:
			if (func7 == 0x01) {
				switch (func3) {
					case 0x00:
						return parse_rtype(RiscVOpcode::MULW, instruction);
					case 0x04:
						return parse_rtype(RiscVOpcode::DIVW, instruction);
					case 0x05:
						return parse_rtype(RiscVOpcode::DIVUW, instruction);
					case 0x06:
						return parse_rtype(RiscVOpcode::REMW, instruction);
					case 0x07:
						return parse_rtype(RiscVOpcode::REMUW, instruction);
				}
				return error_string;
			}
			switch (func3) {
				case 0x00:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::ADDW, instruction);
					else if (func7 == 0x20)
						return parse_rtype(RiscVOpcode::SUBW, instruction);
					return error_string;
				case 0x01:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::SLLW, instruction);
					return error_string;
				case 0x05:
					if (func7 == 0x00)
						return parse_rtype(RiscVOpcode::SRLW, instruction);
					else if (func7 == 0x20)
						return parse_rtype(RiscVOpcode::SRAW, instruction);
					return error_string;
			}
			return error_string;
	}
	return error_string;
}