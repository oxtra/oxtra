#ifndef FD_FADEC_H
#define FD_FADEC_H

#define ARCH_X86_64

#include <cstddef>
#include <cstdint>

#include <array>

namespace x86
{
	// register
	enum Reg : uint8_t {
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
		ip,

		// segment registers
		es = 0, cs, ss, ds, fs, gs,

		// no register speicified
		none = 0x3f
	};

	// instruction (mnemonic)
	enum InstrType : uint16_t {
#define FD_DECODE_TABLE_MNEMONICS
#define FD_MNEMONIC(name, value) name = value,
#include <decode-table.inc>
#undef FD_MNEMONIC
#undef FD_DECODE_TABLE_MNEMONICS
	};

	// flags
	enum InstrFlags : uint8_t {
		lock = 1 << 0,
		rep = 1 << 1,
		repnz = 1 << 2,
		rex = 1 << 3,
		x64 = 1 << 7,
	};


	// operand type
	enum class OpType : uint8_t {
		none,
		reg,
		imm,
		mem,
	};

	enum RegType : uint8_t {
		// register type is encoded in mnemonic
		implicit = 0,

		// low general pupose reigster
		gpl,

		// high general purpose register
		gph,

		// segment register
		seg,

		// fpu register ST(n)
		fpu,

		// mmx reigster
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

	// operand
	class Op {
	public:
		OpType get_type() const {
			return type;
		}

		uint8_t get_size() const {
			return size;
		}

		Reg get_register() const {
			return reg;
		}

		RegType get_register_type() const {
			return reg_type;
		}

	public: //private:
		OpType type;
		uint8_t size;
		Reg reg;
		RegType reg_type;
	};

	class Instr {
	public:
		InstrType get_type() const {
			return type;
		}

		uint8_t  get_size() const {
			return size;
		}

		const Op& get_operand(size_t idx) const {
			/*
			 * this will throw if the idx is invalid
			 * TODO decide if we want to do this
			*/
			return operands.at(idx);
		}

		Reg get_segment() const {
			return segment;
		}

		Reg get_index_register() const {
			return idx_reg;
		}

		uint8_t get_index_scale() const {
			return idx_scale;
		}

		intptr_t get_displacement() const {
			return disp;
		}

		uintptr_t  get_immediate() const {
			return imm;
		}

		uint8_t get_address_size() const {
			return addrsz;
		}

		uint8_t get_operand_size() const {
			return operandsz;
		}

		uint8_t get_flags() const {
			return flags;
		}

		uintptr_t get_address() const {
			return address;
		}

	public://private:
		InstrType type;
		uint8_t flags;
		Reg segment;
		uint8_t addrsz;
		uint8_t operandsz;
		std::array<Op, 4> operands;

		Reg idx_reg;
		uint8_t idx_scale;
		uint8_t size;
		intptr_t disp;
		uintptr_t imm;

		uintptr_t address;
	};

	int decode(const uint8_t* buffer, size_t len_sz, int mode_int, uintptr_t address, x86::Instr& instr);
}

#endif
