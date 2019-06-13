#ifndef FD_FADEC_H
#define FD_FADEC_H

#define ARCH_X86_64

#include <cstddef>
#include <cstdint>

#include <array>

namespace x86
{
	// register
	enum class Register : uint8_t {
		// 64 bit
		rax = 0, rcx, rdx, rbx,
		rsp, rbp, rsi, rdi,

		r0 = 0, r1, r2, r3,
		r4, r5, r6, r7,
		r8, r9, r10, r11,
		r12, r13, r14, r15,

		// byte
		al = 0, cl, dl, bl,
		ah, ch, dh, bh,

		// 16-bit
		ax = 0, cx, dx, bx,
		sp, bp, si, di,

		// 32-bit
		eax, ecx, edx, ebx,
		esp, ebp, esi, edi,

		// instruction pointer
		ip = 0x10,

		// segment registers
		es = 0, cs, ss, ds, fs, gs,

		// no register specified
		none = 0x3f
	};

	// instruction (mnemonic)
	enum class InstructionType : uint16_t {
#define FD_DECODE_TABLE_MNEMONICS
#define FD_MNEMONIC(name, value) name = value,
#include <decode-table.inc>
#undef FD_MNEMONIC
#undef FD_DECODE_TABLE_MNEMONICS
	};

	// flags
	enum class InstructionFlags : uint8_t {
		lock = 1 << 0,
		rep = 1 << 1,
		repnz = 1 << 2,
		rex = 1 << 3,
		x64 = 1 << 7,
	};


	// operand type
	enum class OperandType : uint8_t {
		none,
		reg,
		imm,
		mem,
	};

	enum class RegisterType : uint8_t {
		// register type is encoded in mnemonic
		implicit = 0,

		// low general purpose register
		gpl,

		// high general purpose register
		gph,

		// segment register
		seg,

		// fpu register ST(n)
		fpu,

		// mmx register
		mmx,

		// vector (sse/avx) register xmm/ymm/zmm
		vec,

		// vector mask (avx-512) register Kn
		mask,

		// bound register BNDn
		bnd,

		// control register CRn
		cr,

		// debug register DRn
		dr,
	};

	enum class DecodeMode : int {
		decode_64 = 0,
		decode_32 = 1
	};

	class Operand {
		friend int decode(const uint8_t*, size_t, DecodeMode, uintptr_t, class Instruction&);
		friend void format(const class Instruction&, char*, size_t);
		friend int intern::decode_modrm(const uint8_t*, size_t, DecodeMode, Instruction&, int, bool, Operand*, Operand*);
		friend int intern::decode_prefixes(const uint8_t*, size_t, DecodeMode, int&, uint8_t&, Register&, uint8_t&, int&);
	public:
		/**
		 *
		 * @return The type of the operand. E.g. reg (register), imm (immediate), mem (memory).
		 */
		OperandType get_type() const {
			return type;
		}

		/**
		 *
		 * @return The size of the operand in bytes.
		 */
		uint8_t get_size() const {
			return size;
		}

		/**
		 *
		 * @return The register of an operand.
		 * @remark Only valid if get_type() == OpType::reg.
		 */
		Register get_register() const {
			return reg;
		}

		/**
		 *
		 * @return The type of the register.
		 * @remark Only valid if get_type() == OpType::reg. Needed for example to distinguish high-byte registers.
		 */
		RegisterType get_register_type() const {
			return reg_type;
		}

	private:
		OperandType type;
		uint8_t size;
		Register reg;
		RegisterType reg_type;
	};

	class Instruction {
		friend int decode(const uint8_t*, size_t, DecodeMode, uintptr_t, class Instruction&);
		friend void format(const class Instruction&, char*, size_t);
		friend int intern::decode_modrm(const uint8_t*, size_t, DecodeMode, Instruction&, int, bool, Operand*, Operand*);
		friend int intern::decode_prefixes(const uint8_t*, size_t, DecodeMode, int&, uint8_t&, Register&, uint8_t&, int&);
	public:
		/**
		 *
		 * @return The type/mnemonic of the instruction.
		 */
		InstructionType get_type() const {
			return type;
		}

		/**
		 *
		 * @return The size of the instruction in bytes.
		 */
		uint8_t get_size() const {
			return size;
		}

		/**
		 *
		 * @param idx The index of the operand.
		 * @return The Operand at the index.
		 */
		const Operand& get_operand(size_t idx) const {
			return operands[idx];
		}

		/**
		 *
		 * @return The segment override or Reg::none if no segment override was specified.
		 */
		Register get_segment() const {
			return segment;
		}

		/**
		 *
		 * @return The index register of the [base+index*scale+displacement] addressing mode.
		 * @remark Only valid if an operand is of type OpType::mem.
		 */
		Register get_index_register() const {
			return idx_reg;
		}

		/**
		 *
		 * @return The scale of the [base+index*scale+displacement] addressing mode.
		 * @remark Only valid if an operand is of type OpType::mem and get_index_register() != Reg::none.
		 */
		uint8_t get_index_scale() const {
			return 1 << idx_scale;
		}

		/**
		 *
		 * @return The displacement of the [base+index*scale+displacement] addressing mode.
		 * @remark Only valid if an operand is of type OpType::mem.
		 */
		intptr_t get_displacement() const {
			return disp;
		}

		/**
		 *
		 * @return The value of an immediate operand.
		 * @remark Only valid if an operand is of type OpType::imm.
		 */
		uintptr_t get_immediate() const {
			return imm;
		}

		/**
		 *
		 * @return The address size attribute of the instruction in bytes.
		 */
		uint8_t get_address_size() const {
			return addrsz;
		}

		/**
		 *
		 * @return The operand width in bytes.
		 * @remark Needed for MOVS for example.
		 */
		uint8_t get_operand_size() const {
			return operandsz;
		}

		/**
		 *
		 * @return True if the instruction has a rep prefix.
		 */
		bool has_rep() const {
			return flags & static_cast<uint8_t>(InstructionFlags::rep);
		}

		/**
		 *
		 * @return True if the instruction has a repnz prefix.
		 */
		bool has_repnz() const {
			return flags & static_cast<uint8_t>(InstructionFlags::repnz);
		}

		/**
		 *
		 * @return True if the instruction has a lock prefix.
		 */
		bool has_lock() const {
			return flags & static_cast<uint8_t>(InstructionFlags::lock);
		}

		/**
		 *
		 * @return True if the instruction is a 64-bit instruction.
		 */
		bool is_64() const {
			return flags & static_cast<uint8_t>(InstructionFlags::x64);
		}

		/**
		 *
		 * @return The address of the instruction.
		 */
		uintptr_t get_address() const {
			return address;
		}

	private:
		InstructionType type;
		uint8_t flags;
		Register segment;
		uint8_t addrsz;
		uint8_t operandsz;
		std::array<Operand, 4> operands;

		Register idx_reg;
		uint8_t idx_scale;
		uint8_t size;
		intptr_t disp;
		uintptr_t imm;

		uintptr_t address;
	};

	/**
	 *
	 * @param buffer Buffer containing the instructions in bytes.
	 * @param len_sz Length of the buffer.
	 * @param mode Decoding mode.
	 * @param address Virtual address  where the instruction is located.
	 * @param instr Reference to the instruction buffer.
	 * @return The number of bytes consumed by the instruction or a negative number indicating an error.
	 */
	int decode(const uint8_t* buffer, size_t len_sz, DecodeMode mode, uintptr_t address, Instruction& instr);

	/**
	 *
	 * @param instr Instruction to format.
	 * @param buffer Buffer holding the formatted instruction.
	 * @param len Length of the buffer.
	 */
	void format(const Instruction& instr, char* buffer, size_t len);

	namespace intern {
		int decode_modrm(const uint8_t* buffer, size_t len, DecodeMode mode, Instruction& instr, int prefixes, bool vsib, Operand* o1, Operand* o2);
		int decode_prefixes(const uint8_t* buffer, size_t len, DecodeMode mode, int& prefixes,
							   uint8_t& mandatory, Register& segment, uint8_t& vex_operand, int& opcode_escape);
	}
}

#endif
