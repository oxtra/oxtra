#include <fadec.h>

#if defined(ARCH_X86_64) && __SIZEOF_POINTER__ < 8
#error "Decoding x86-64 requires a 64-bit architecture"
#endif

#define LIKELY(x) (x)//__builtin_expect((x), 1)
#define UNLIKELY(x) (x)//__builtin_expect((x), 0)

#define FD_DECODE_TABLE_DATA
static const uint8_t _decode_table[] = {
#include "decode-table.inc"
};
#undef FD_DECODE_TABLE_DATA

// Defines FD_TABLE_OFFSET_32 and FD_TABLE_OFFSET_64, if available
#define FD_DECODE_TABLE_DEFINES

#include "decode-table.inc"

#undef FD_DECODE_TABLE_DEFINES

using namespace fadec;

enum TableEntry : uint32_t {
	entry_none = 0,
	entry_instr,
	entry_table256,
	entry_table8,
	entry_table72,
	entry_table_prefix,
	entry_table_vex,
	entry_mask
};

inline void entry_unpack(const uint16_t*& table, uint32_t& kind, uint16_t entry) {
	const auto entry_copy = entry;
	table = reinterpret_cast<const uint16_t*>(&_decode_table[entry_copy & ~7]);
	kind = entry_copy & entry_mask;
}

#define LOAD_LE_1(buf) ((size_t) *(uint8_t*) (buf))
#define LOAD_LE_2(buf) (LOAD_LE_1(buf) | LOAD_LE_1((uint8_t*) (buf) + 1)<<8)
#define LOAD_LE_3(buf) (LOAD_LE_2(buf) | LOAD_LE_1((uint8_t*) (buf) + 2)<<16)
#define LOAD_LE_4(buf) (LOAD_LE_2(buf) | LOAD_LE_2((uint8_t*) (buf) + 2)<<16)
#if defined(ARCH_X86_64)
#define LOAD_LE_8(buf) (LOAD_LE_4(buf) | LOAD_LE_4((uint8_t*) (buf) + 4)<<32)
#endif

enum PrefixSet {
	prefix_none = 0,
	prefix_lock = static_cast<int>(InstructionFlags::lock),
	prefix_rep = static_cast<int>(InstructionFlags::rep),
	prefix_repnz = static_cast<int>(InstructionFlags::repnz),
	prefix_rex = static_cast<int>(InstructionFlags::rex),
	prefix_opsz = 1 << 13,
	prefix_addrsz = 1 << 14,
	prefix_rexb = 1 << 15,
	prefix_rexx = 1 << 16,
	prefix_rexr = 1 << 17,
	prefix_rexw = 1 << 18,
	prefix_vexl = 1 << 19,
	prefix_vex = 1 << 20,
};

int fadec::decode_prefixes(const uint8_t* buffer, size_t len, DecodeMode mode, int& prefixes,
						   uint8_t& mandatory, Register& segment, uint8_t& vex_operand, int& opcode_escape) {

	size_t off = 0;
	prefixes = prefix_none;

	uint8_t rep = 0;
	mandatory = 0;
	segment = Register::none;
	opcode_escape = -1;

	while (LIKELY(off < len)) {
		const auto prefix = buffer[off];
		switch (prefix) {
			default:
				goto out;

				// From segment overrides, the last one wins.
			case 0x26:
				segment = Register::es;
				off++;
				break;
			case 0x2e:
				segment = Register::cs;
				off++;
				break;
			case 0x3e:
				segment = Register::ds;
				off++;
				break;
			case 0x64:
				segment = Register::fs;
				off++;
				break;
			case 0x65:
				segment = Register::gs;
				off++;
				break;
			case 0x67:
				prefixes |= prefix_addrsz;
				off++;
				break;
			case 0xf0:
				prefixes |= prefix_lock;
				off++;
				break;
			case 0x66:
				prefixes |= prefix_opsz;
				off++;
				break;
				// From REP/REPE and REPNZ, the last one wins; and for mandatory
				// prefixes they have a higher priority than 66h (handled below).
			case 0xf3:
				rep = prefix_rep;
				mandatory = 2;
				off++;
				break;
			case 0xf2:
				rep = prefix_repnz;
				mandatory = 3;
				off++;
				break;

#if defined(ARCH_X86_64)
			case 0x40:
			case 0x41:
			case 0x42:
			case 0x43:
			case 0x44:
			case 0x45:
			case 0x46:
			case 0x47:
			case 0x48:
			case 0x49:
			case 0x4a:
			case 0x4b:
			case 0x4c:
			case 0x4d:
			case 0x4e:
			case 0x4f:
				if (mode == DecodeMode::decode_64) {
					prefixes |= prefix_rex;
					prefixes |= prefix & 0x1 ? prefix_rexb : 0;
					prefixes |= prefix & 0x2 ? prefix_rexx : 0;
					prefixes |= prefix & 0x4 ? prefix_rexr : 0;
					prefixes |= prefix & 0x8 ? prefix_rexw : 0;
					off++;
				}
				// If in 64-bit mode, the REX prefix is always the last prefix. In
				// 32-bit mode these are regular opcodes, so exit without consuming.
				goto out;
#endif

			case 0xc4:
			case 0xc5: { // VEX
				if (UNLIKELY(off + 1 >= len))
					return -1;

				auto byte = buffer[off + 1];
				if (mode == DecodeMode::decode_32 && (byte & 0xc0) != 0xc0)
					goto out;

				prefixes |= prefix_vex;
				prefixes |= (byte & 0x80) ? 0 : prefix_rexr;
				if (prefix == 0xc4) { // 3-byte VEX
					prefixes |= (byte & 0x80) ? 0 : prefix_rexr;
					prefixes |= (byte & 0x40) ? 0 : prefix_rexx;

					// SDM Vol 2A 2-15 (Dec. 2016): Ignored in 32-bit mode
					prefixes |= (mode == DecodeMode::decode_64 || (byte & 0x20)) ? 0 : prefix_rexb;
					opcode_escape = (byte & 0x1f);

					// Load third byte of VEX prefix
					if (UNLIKELY(off + 2 >= len))
						return -1;

					byte = buffer[off + 2];
					// SDM Vol 2A 2-16 (Dec. 2016) says that:
					// - "In 32-bit modes, VEX.W is silently ignored."
					// - VEX.W either replaces REX.W, is don't care or is reserved.
					// This is actually incorrect, there are instructions that
					// use VEX.W as an opcode extension even in 32-bit mode.
					prefixes |= (byte & 0x80) ? prefix_rexw : 0;
				} else { // 2-byte VEX
					opcode_escape = 1;
				}

				prefixes |= (byte & 0x04) ? prefix_vexl : 0;
				mandatory = (byte & 0x03);
				vex_operand = ((byte & 0x78) >> 3) ^ 0xf;

				// VEX prefix is always the last prefix.
				off += (prefix == 0xc4) ? 3 : 2;
				goto out;
			}

		}
	}
	out:
	// If there is no REP/REPNZ prefix and implied opcode extension from a VEX
	// prefix, offer 66h as mandatory prefix. If there is a REP prefix, then the
	// 66h prefix is ignored when evaluating mandatory prefixes.
	if (mandatory == 0 && (prefixes & prefix_opsz))
		mandatory = 1;

	prefixes |= rep;

	return off;
}

int fadec::decode_modrm(const uint8_t* buffer, size_t len, DecodeMode mode, Instruction& instr, int prefixes, bool vsib,
						Operand* o1, Operand* o2) {
	size_t off = 0;

	if (UNLIKELY(off >= len))
		return -1;

	const auto modrm = buffer[off++];
	const auto mod = (modrm & 0xc0) >> 6;
	const auto mod_reg = (modrm & 0x38) >> 3;
	const auto rm = (modrm & 0x07);

	// Operand 2 may be NULL when reg field is used as opcode extension
	if (o2) {
		auto reg_idx = mod_reg;

#if defined(ARCH_X86_64)
		reg_idx += (prefixes & prefix_rexr) ? 8 : 0;
#endif

		o2->type = OperandType::reg;
		o2->reg = static_cast<Register>(reg_idx);
	}

	if (mod == 3) {
		auto reg_idx = rm;

#if defined(ARCH_X86_64)
		reg_idx += (prefixes & prefix_rexb) ? 8 : 0;
#endif

		o1->type = OperandType::reg;
		o1->reg = static_cast<Register>(reg_idx);

		return off;
	}

	// sib byte
	uint8_t scale = 0;
	uint8_t idx = 4;
	uint8_t base = rm;
	if (rm == 4) {
		if (UNLIKELY(off >= len))
			return -1;

		const auto sib = buffer[off++];
		scale = (sib & 0xc0) >> 6;
		idx = (sib & 0x38) >> 3;

#if defined(ARCH_X86_64)
		idx += (prefixes & prefix_rexx) ? 8 : 0;
#endif

		base = (sib & 0x07);
	}

	o1->type = OperandType::mem;
	instr.idx_scale = scale;
	instr.idx_reg = (!vsib && idx == 4) ? Register::none : static_cast<Register>(idx);

	// RIP-relative addressing only if SIB-byte is absent
	if (mod == 0 && rm == 5 && mode == DecodeMode::decode_64)
		o1->reg = Register::ip;
	else if (mod == 0 && base == 5)
		o1->reg = Register::none;
	else
		o1->reg = static_cast<Register>(base + (prefixes & prefix_rexb ? 8 : 0));

	if (mod == 1) {
		if (UNLIKELY(off + 1 > len))
			return -1;

		instr.disp = static_cast<int8_t>(LOAD_LE_1(&buffer[off]));
		off += 1;
	} else if (mod == 2 || (mod == 0 && base == 5)) {
		if (UNLIKELY(off + 4 > len))
			return -1;

		instr.disp = static_cast<int32_t>(LOAD_LE_4(&buffer[off]));
		off += 4;
	} else {
		instr.disp = 0;
	}

	return off;
}

struct InstrDesc {
	InstructionType type;
	uint8_t operand_indices;
	uint8_t operand_sizes;
	uint8_t immediate;
	uint8_t gp_size_8 : 1;
	uint8_t gp_size_def64 : 1;
	uint8_t gp_instr_width : 1;
	uint8_t gp_fixed_operand_size : 3;
	uint8_t lock : 1;
	uint8_t vsib : 1;
	uint16_t reg_types;

	bool has_modrm() const {
		return (operand_indices & (3 << 0)) != 0;
	}

	uint8_t modrm_idx() const {
		return ((operand_indices >> 0) & 3) ^ 3;
	}

	bool has_modreg() const {
		return (operand_indices & (3 << 2)) != 0;
	}

	uint8_t modreg_idx() const {
		return ((operand_indices >> 2) & 3) ^ 3;
	}

	bool has_vexreg() const {
		return (operand_indices & (3 << 4)) != 0;
	}

	uint8_t vexreg_idx() const {
		return ((operand_indices >> 4) & 3) ^ 3;
	}

	bool has_implicit() const {
		return (operand_indices & (3 << 6)) != 0;
	}

	uint8_t implicit_idx() const {
		return ((operand_indices >> 6) & 3) ^ 3;
	}

	uint8_t imm_control() const {
		return ((immediate >> 4) & 0x7);
	}

	uint8_t imm_index() const {
		return ((immediate & 3) ^ 3);
	}

	uint8_t imm_byte() const {
		return ((immediate >> 7) & 1);
	}
};

int fadec::decode(const uint8_t* buffer, size_t len_sz, DecodeMode mode, uintptr_t address, Instruction& instr) {
	const uint16_t* table = nullptr;

	size_t len = len_sz > 15 ? 15 : len_sz;

#if defined(ARCH_386)
	if (mode == DecodeMode::decode_32)
		table = reinterpret_cast<const uint16_t*>(&_decode_table[FD_TABLE_OFFSET_32]);
#endif
#if defined(ARCH_X86_64)
	if (mode == DecodeMode::decode_64)
		table = reinterpret_cast<const uint16_t*>(&_decode_table[FD_TABLE_OFFSET_64]);
#endif

	if (UNLIKELY(table == nullptr))
		return -2;

	size_t off = 0;
	uint8_t vex_operand = 0;
	uint8_t mandatory_prefix;
	int opcode_escape;
	auto prefixes = 0;

	const auto retval = decode_prefixes(buffer + off, len - off, mode, prefixes, mandatory_prefix, instr.segment,
										vex_operand, opcode_escape);

	if (UNLIKELY(retval < 0 || off + retval >= len))
		return -1;

	off += retval;

	uint32_t kind = entry_table256;

	// "Legacy" walk through table and escape opcodes
	if (LIKELY(opcode_escape < 0))
		while (kind == entry_table256 && LIKELY(off < len))
			entry_unpack(table, kind, table[buffer[off++]]);

		// VEX/EVEX compact escapes; the prefix precedes the single opcode byte
	else if (opcode_escape == 1 || opcode_escape == 2 || opcode_escape == 3) {
		entry_unpack(table, kind, table[0x0f]);
		if (opcode_escape == 2)
			entry_unpack(table, kind, table[0x38]);

		else if (opcode_escape == 3)
			entry_unpack(table, kind, table[0x3A]);

		if (LIKELY(off < len))
			entry_unpack(table, kind, table[buffer[off++]]);
	} else
		return -1;

	// Then, walk through ModR/M-encoded opcode extensions.
	if ((kind == entry_table8 || kind == entry_table72) && LIKELY(off < len)) {
		uint16_t entry = 0;
		if (kind == entry_table72 && (buffer[off] & 0xc0) == 0xc0) {
			entry = table[buffer[off] - 0xb8];
			if ((entry & entry_mask) != entry_none)
				off++;
			else
				entry = table[(buffer[off] >> 3) & 7];
		} else
			entry = table[(buffer[off] >> 3) & 7];

		entry_unpack(table, kind, entry);
	}

	// Handle mandatory prefixes (which behave like an opcode ext.).
	if (kind == entry_table_prefix) {
		auto index = mandatory_prefix;
		index |= (prefixes & prefix_vex ? (1 << 2) : 0);

		// If a prefix is mandatory and used as opcode extension, it has no
		// further effect on the instruction. This is especially important
		// for the 0x66 prefix, which could otherwise override the operand
		// size of general purpose registers.
		prefixes &= ~(prefix_opsz | prefix_repnz | prefix_rep);
		entry_unpack(table, kind, table[index]);
	}

	// For VEX prefix, we have to distinguish between VEX.W and VEX.L which may
	// be part of the opcode.
	if (kind == entry_table_vex) {
		uint8_t index = 0;
		index |= (prefixes & prefix_rexw ? (1 << 0) : 0);
		index |= (prefixes & prefix_vexl ? (1 << 1) : 0);
		entry_unpack(table, kind, table[index]);
	}

	if (UNLIKELY(kind != entry_instr))
		return -1;

	const auto desc = reinterpret_cast<const InstrDesc*>(table);

	instr.type = desc->type;
	instr.flags = prefixes & 0x7f;

	if (mode == DecodeMode::decode_64)
		instr.flags |= static_cast<int>(InstructionFlags::x64);

	instr.address = address;

	uint8_t op_size = 0;
	if (desc->gp_size_8)
		op_size = 1;

	else if (mode == DecodeMode::decode_64 && (prefixes & prefix_rexw))
		op_size = 8;

	else if (prefixes & prefix_opsz)
		op_size = 2;

	else if (mode == DecodeMode::decode_64 && desc->gp_size_def64)
		op_size = 8;

	else
		op_size = 4;

	instr.operandsz = desc->gp_instr_width ? op_size : 0;

	uint8_t vec_size = 16;
	if (prefixes & prefix_vexl)
		vec_size = 32;

	// Compute address size.
	uint8_t addr_size = (mode == DecodeMode::decode_64) ? 8 : 4;
	if (prefixes & prefix_addrsz)
		addr_size >>= 1;

	instr.addrsz = addr_size;

	uint8_t operand_sizes[4] = {
			0, static_cast<uint8_t>(1 << desc->gp_fixed_operand_size), op_size, vec_size
	};

	__builtin_memset(instr.operands.data(), 0, sizeof(instr.operands));
	for (auto i = 0; i < 4; i++) {
		const uint8_t enc_size = (desc->operand_sizes >> 2 * i) & 3;
		instr.operands[i].size = operand_sizes[enc_size];
	}

	if (desc->has_implicit()) {
		auto& operand = instr.operands[desc->implicit_idx()];
		operand.type = OperandType::reg;
		operand.reg = Register::rax;
	}

	if (desc->has_modrm()) {
		auto op1 = &instr.operands[desc->modrm_idx()];
		auto op2 = desc->has_modreg() ? &instr.operands[desc->modreg_idx()] : nullptr;

		const auto retval = decode_modrm(buffer + off, len - off, mode, instr, prefixes, desc->vsib, op1, op2);
		if (UNLIKELY(retval < 0))
			return -1;

		off += retval;
	} else if (desc->has_modreg()) {
		// If there is no ModRM, but a Mod-Reg it's opcode encoded.
		auto& op = instr.operands[desc->modreg_idx()];

		uint8_t reg_idx = buffer[off - 1] & 7;
#if defined(ARCH_X86_64)
		reg_idx += (prefixes & prefix_rexb) ? 8 : 0;
#endif

		op.type = OperandType::reg;
		op.reg = static_cast<Register>(reg_idx);
	}

	if (UNLIKELY(desc->has_vexreg())) {
		auto& op = instr.operands[desc->vexreg_idx()];
		op.type = OperandType::reg;
		op.reg = static_cast<Register>(vex_operand);
	}

	const auto imm_control = static_cast<uint32_t>(desc->imm_control());
	if (imm_control == 1) {
		auto& op = instr.operands[desc->imm_index()];
		op.type = OperandType::imm;
		op.size = 1;
		instr.imm = 1;
	} else if (imm_control == 2) {
		auto& op = instr.operands[desc->imm_index()];
		op.type = OperandType::mem;
		op.reg = Register::none;
		op.size = op_size;
		instr.idx_reg = Register::none;

		if (UNLIKELY(off + addr_size > len))
			return -1;

#if defined(ARCH_386)
		if (addr_size == 2)
			instr.disp = LOAD_LE_2(&buffer[off]);
#endif
		if (addr_size == 4)
			instr.disp = LOAD_LE_4(&buffer[off]);

#if defined(ARCH_X86_64)
		if (addr_size == 8)
			instr.disp = LOAD_LE_8(&buffer[off]);
#endif

		off += addr_size;
	} else if (imm_control != 0) {
		auto& op = instr.operands[desc->imm_index()];

		uint8_t imm_size = 0;
		if (desc->imm_byte())
			imm_size = 1;

		else if (UNLIKELY(instr.type == InstructionType::RET_IMM))
			imm_size = 2;

		else if (UNLIKELY(instr.type == InstructionType::ENTER))
			imm_size = 3;

#if defined(ARCH_X86_64)
		else if (mode == DecodeMode::decode_64 && UNLIKELY(imm_control == 4))
			// Jumps are always 8 or 32 bit on x86-64
			imm_size = 4;
#endif

		else if (prefixes & prefix_opsz)
			imm_size = 2;

#if defined(ARCH_X86_64)
		else if (mode == DecodeMode::decode_64 && (prefixes & prefix_rexw && instr.type == InstructionType::MOVABS_IMM))
			imm_size = 8;
#endif

		else
			imm_size = 4;

		if (UNLIKELY(off + imm_size > len))
			return -1;

		if (imm_size == 1)
			instr.imm = static_cast<int8_t>(LOAD_LE_1(&buffer[off]));

		else if (imm_size == 2)
			instr.imm = static_cast<int16_t>(LOAD_LE_2(&buffer[off]));

		else if (imm_size == 3)
			instr.imm = LOAD_LE_3(&buffer[off]);

		else if (imm_size == 4)
			instr.imm = static_cast<int32_t>(LOAD_LE_4(&buffer[off]));

#if defined(ARCH_X86_64)
		else if (imm_size == 8)
			instr.imm = static_cast<int64_t>(LOAD_LE_8(&buffer[off]));
#endif

		off += imm_size;

		if (imm_control == 4) {
			instr.imm += instr.address + off;

#if defined(ARCH_X86_64)
			// On x86-64, jumps always have an operand size of 64 bit.
			if (mode == DecodeMode::decode_64)
				op.size = 8;

#endif
		}

		if (UNLIKELY(imm_control == 5)) {
			op.type == OperandType::reg;
			op.reg = static_cast<Register>((instr.imm & 0xf0) >> 4);
		} else {
			op.type = OperandType::imm;
		}
	}

	if ((prefixes & prefix_lock) && !desc->lock)
		return -1;

	if ((prefixes & prefix_lock) & instr.operands[0].type != OperandType::mem)
		return -1;

	for (auto i = 0; i < 4; i++) {
		auto& op = instr.operands[i];

		if (op.type == OperandType::mem && op.reg == Register::ip) {
			op.reg = Register::none;
			instr.disp += instr.address + off;
		} else if (op.type != OperandType::reg)
			continue;

		auto reg_type = static_cast<RegisterType>((desc->reg_types >> 4 * i) & 0xf);
		if (reg_type == RegisterType::gpl && !(prefixes & prefix_rex) && op.size == 1 &&
			static_cast<uint8_t>(op.reg) >= 4)
			reg_type = RegisterType::gph;

		op.reg_type = reg_type;
	}

	instr.size = off;

	return off;
}

#define FD_DECODE_TABLE_STRTAB1
static const char* _mnemonic_str =
#include <decode-table.inc>
;
#undef FD_DECODE_TABLE_STRTAB1

#define FD_DECODE_TABLE_STRTAB2
static const uint16_t _mnemonic_offs[] = {
#include <decode-table.inc>
};
#undef FD_DECODE_TABLE_STRTAB2

#define fmt_concat(...) { \
    buf += snprintf(buf, end - buf, __VA_ARGS__); \
    if (buf > end) \
        buf = end; \
}

static void format_instruction(char*& buf, char*& end, const Instruction& instr) {
	// extract the string
	const char* string = &_mnemonic_str[_mnemonic_offs[static_cast<uint16_t>(instr.get_type())]];

	// convert the string
	char buffer[256];
	char* curr_buffer = buffer;
	while (string[0]) {
		if (string[0] == '_')
			break;
		if (string[0] >= 'A' && string[0] <= 'Z')
			curr_buffer[0] = string[0] - 'A' + 'a';
		else
			curr_buffer[0] = string[0];
		string++;
		curr_buffer++;
	}
	curr_buffer[0] = '\0';
	fmt_concat("%s", buffer);
	if (instr.get_operand_size()) fmt_concat("_%u", instr.get_operand_size())
}

static void format_register(char*& buf, char*& end, fadec::Register reg, fadec::RegisterType type, uint8_t size) {
	static constexpr const char* string_map[] = {
			"rax", "rcx", "rdx", "rbx",
			"rsp", "rbp", "rsi", "rdi",
			"r8", "r9", "r10", "r11",
			"r12", "r13", "r14", "r15",

			"eax", "ecx", "edx", "ebx",
			"esp", "ebp", "esi", "edi",
			"r8d", "r9d", "r10d", "r11d",
			"r12d", "r13d", "r14d", "r15d",

			"ax", "cx", "dx", "bx",
			"sp", "bp", "si", "di",
			"r8w", "r9w", "r10w", "r11w",
			"r12w", "r13w", "r14w", "r15w",

			"al", "cl", "dl", "bl",
			"spl", "bpl", "sil", "dil",
			"r8l", "r9l", "r10l", "r11l",
			"r12l", "r13l", "r14l", "r15l",

			"ah", "ch", "dh", "bh",
			"sph", "bph", "sih", "dih",
			"r8h", "r9h", "r10h", "r11h",
			"r12h", "r13h", "r14h", "r15h"
	};

	if (type == RegisterType::gph && static_cast<uintptr_t>(reg) <= 15) fmt_concat(string_map[static_cast<uintptr_t>(reg) + 64])
	else if (type == RegisterType::gpl && static_cast<uintptr_t>(reg) <= 15) {
		switch (size) {
			case 8: fmt_concat(string_map[static_cast<uintptr_t>(reg)])
				break;
			case 4: fmt_concat(string_map[static_cast<uintptr_t>(reg) + 16])
				break;
			case 2: fmt_concat(string_map[static_cast<uintptr_t>(reg) + 32])
				break;
			case 1:
			default: fmt_concat(string_map[static_cast<uintptr_t>(reg) + 48])
				break;

		}
	} else fmt_concat("r%u", static_cast<unsigned int>(reg))
}

void fadec::format(const Instruction& instr, char* buffer, size_t len) {
	auto buf = buffer;
	auto end = buffer + len;

	if (instr.has_rep()) fmt_concat("rep ")

	if (instr.has_repnz()) fmt_concat("repnz ")

	if (static_cast<uint8_t>(instr.get_segment()) < 6) fmt_concat("%cs:",
																  "ecsdfg"[static_cast<uint8_t>(instr.get_segment())]);

	if (instr.is_64() && instr.get_address_size() == 4) fmt_concat("addr32:")

	else if (!instr.is_64() && instr.get_address_size() == 2) fmt_concat("addr16:")

	if (instr.has_lock()) fmt_concat("lock ")

	format_instruction(buf, end, instr);

	for (size_t i = 0; i < 4; ++i) {
		auto&& operand = instr.get_operand(i);

		const auto op_type = operand.get_type();
		if (op_type == OperandType::none)
			break;

		fmt_concat(i > 0 ? ", " : " ")

		switch (op_type) {
			case OperandType::reg:
				format_register(buf, end, operand.get_register(), operand.get_register_type(), operand.get_size());
				break;
			case OperandType::imm: {
				auto immediate = instr.get_immediate();
				if (instr.get_operand_size() == 1)
					immediate &= 0xff;

				else if (instr.get_operand_size() == 2)
					immediate &= 0xffff;

				else if (instr.get_operand_size() == 4)
					immediate &= 0xffffffff;

				fmt_concat("0x%lx", immediate)
				break;
			}
			case OperandType::mem: {
				fmt_concat("[")
				const auto base = operand.get_register();
				const auto idx = instr.get_index_register();
				const auto disp = instr.get_displacement();

				if (base != Register::none) {
					format_register(buf, end, base, RegisterType::gpl, instr.get_address_size());
					if (idx != Register::none) fmt_concat("+")
					else if(disp > 0) fmt_concat("+")
					else if(disp < 0) fmt_concat("-")
				}

				if (idx != Register::none) {
					format_register(buf, end, idx, RegisterType::gpl, instr.get_address_size());
					fmt_concat("*%u", 1 << instr.get_index_scale())

					//fmt_concat("%u*r%u", 1 << instr.get_index_scale(), static_cast<unsigned int>(instr.get_index_register()))

					if(disp > 0) fmt_concat("+")
					else if(disp < 0) fmt_concat("-")
				}

				if (disp < 0) {
					if(base == Register::none && idx == Register::none) fmt_concat("-0x%lx", -disp)
					else fmt_concat("0x%lx", -disp)
				}

				else if (disp > 0 || (base == Register::none && idx == Register::none)) fmt_concat("0x%lx", disp)
				fmt_concat("]")
			}

			case OperandType::none:
			default:
				break;
		}
	}
}