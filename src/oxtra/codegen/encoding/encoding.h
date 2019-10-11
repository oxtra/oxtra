#ifndef OXTRA_ENCODING_H
#define OXTRA_ENCODING_H

/**
 * documentation in this file assumes a 64 bit system
 */

#include <cstdint>
#include "oxtra/utils/types.h"

namespace encoding {

	enum class RiscVRegister {
		zero = 0,	// hardwired zero
		ra,			// return address
		sp,			// stack pointer
		gp,			// global pointer
		tp,			// thread pointer
		t0,			// temporary/alternate link register
		t1, t2,		// temporaries
		s0,    		// saved register/frame pointer
		s1,    		// saved register
		a0, a1,		// function arguments/return values
		a2, a3, a4, a5, a6, a7, // function arguments
		s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, // saved registers
		t3, t4, t5, t6 // temporaries
	};

	enum class RiscVFloatingRegister {
		f0, f1, f2, f3, f4, f5, f6, f7, f8, f9,
		f10, f11, f12, f13, f14, f15, f16, f17, f18, f19,
		f20, f21, f22, f23, f24, f25, f26, f27, f28, f29,
		f30, f31,
		fcsr // control and status register
	};

	union RType {
		struct {
			uint32_t opcode : 7;
			uint32_t rd : 5;
			uint32_t funct3 : 3;
			uint32_t rs1 : 5;
			uint32_t rs2 : 5;
			uint32_t funct7 : 7;
		};
		uint32_t raw;
	};

	union IType {
		struct {
			uint32_t opcode : 7;
			uint32_t rd : 5;
			uint32_t funct3 : 3;
			uint32_t rs1 : 5;
			uint32_t imm : 12;
		};
		uint32_t raw;
	};

	union SType {
		struct {
			uint32_t opcode : 7;
			uint32_t immlow : 5;
			uint32_t funct3 : 3;
			uint32_t rs1 : 5;
			uint32_t rs2 : 5;
			uint32_t immhigh : 7;
		};
		uint32_t raw;
	};

	union UType {
		struct {
			uint32_t opcode : 7;
			uint32_t rd : 5;
			uint32_t imm : 20;
		};
		uint32_t raw;
	};

	union BType {
		struct {
			uint32_t opcode : 7;
			uint32_t imm_11 : 1;
			uint32_t imm_4_1 : 4;
			uint32_t funct3 : 3;
			uint32_t rs1 : 5;
			uint32_t rs2 : 5;
			uint32_t imm_10_5 : 6;
			uint32_t imm_12 : 1;
		};
		uint32_t raw;
	};

	union JType {
		struct {
			uint32_t opcode : 7;
			uint32_t rd : 5;
			uint32_t imm_19_12 : 8;
			uint32_t imm_11 : 1;
			uint32_t imm_10_1 : 10;
			uint32_t imm_20 : 1;
		};
		uint32_t raw;
	};

	union R4Type {
		struct {
			uint32_t opcode : 7;
			uint32_t rd : 5;
			uint32_t funct3 : 3;
			uint32_t rs1 : 5;
			uint32_t rs2 : 5;
			uint32_t funct2 : 2;
			uint32_t rs3 : 5;
		};
		uint32_t raw;
	};

	/* --- RV32I Base Instruction Set --------------------------------------------------------------------------------- */

	/**
	* rd[31:12] = imm
	* rd[11:0] = 0
	*/
	utils::riscv_instruction_t LUI(RiscVRegister rd, uint32_t imm);

	/**
	* rd = pc + (imm << 12)
	*/
	utils::riscv_instruction_t AUIPC(RiscVRegister rd, uint32_t imm);

	/**
	* rd = pc + 4
	* pc += offset << 1 (offset must be even)
	*/
	utils::riscv_instruction_t JAL(RiscVRegister rd, uint32_t offset);

	/**
	* rd = pc + 4
	* pc = (rs1 + offset) & !1
	*/
	utils::riscv_instruction_t JALR(RiscVRegister rd, RiscVRegister rs1, uint16_t offset);

	/**
	* pc += (rs1 == rs2) ? offset : 0
	*/
	utils::riscv_instruction_t BEQ(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* pc += (rs1 != rs2) ? offset : 0
	*/
	utils::riscv_instruction_t BNE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* pc += (rs1 < rs2) ? offset : 0 (signed)
	*/
	utils::riscv_instruction_t BLT(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* pc += (rs1 >= rs2) ? offset : 0 (signed)
	*/
	utils::riscv_instruction_t BGE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* pc += (rs1 < rs2) ? offset : 0 (unsigned)
	*/
	utils::riscv_instruction_t BLTU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* pc += (rs1 >= rs2) ? offset : 0 (unsigned)
	*/
	utils::riscv_instruction_t BGEU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* rd = [rs1 + imm] (8 bit sign extended)
	*/
	utils::riscv_instruction_t LB(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = [rs1 + imm] (16 bit sign extended)
	*/
	utils::riscv_instruction_t LH(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = [rs1 + imm] (32 bit sign extended)
	*/
	utils::riscv_instruction_t LW(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**	* rd = [rs1 + imm] (64 bit)
	*/
	utils::riscv_instruction_t LD(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = [rs1 + imm] (8 bit zero extended)
	*/
	utils::riscv_instruction_t LBU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = [rs1 + imm] (16 bit zero extended)
	*/
	utils::riscv_instruction_t LHU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = [rs1 + imm] (32 bit zero extended)
	*/
	utils::riscv_instruction_t LWU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* [rs1 + imm] = rs2[7:0]
	*/
	utils::riscv_instruction_t SB(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm);

	/**
	* [rs1 + imm] = rs2[15:0]
	*/
	utils::riscv_instruction_t SH(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm);

	/**
	* [rs1 + imm] = rs2[31:0]
	*/
	utils::riscv_instruction_t SW(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm);

	/**
	* [rs1 + imm] = rs2
	*/
	utils::riscv_instruction_t SD(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm);

	/**
	* rd = rs1 + imm (ignore overflow)
	*/
	utils::riscv_instruction_t ADDI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = (rs1 < imm) ? 1 : 0 (signed)
	*/
	utils::riscv_instruction_t SLTI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = (rs1 < imm) ? 1 : 0 (unsigned)
	*/
	utils::riscv_instruction_t SLTIU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = rs1 ⊕ imm
	*/
	utils::riscv_instruction_t XORI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = rs1 | imm
	*/
	utils::riscv_instruction_t ORI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = rs1 & imm
	*/
	utils::riscv_instruction_t ANDI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm);

	/**
	* rd = rs1 << shamt[5:0]
	*/
	utils::riscv_instruction_t SLLI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt);

	/**
	* rd = rs1 >> shamt[5:0] (logical)
	*/
	utils::riscv_instruction_t SRLI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt);

	/**
	* rd = rs1 >> shamt[5:0] (arithmetic)
	*/
	utils::riscv_instruction_t SRAI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt);

	/*
	* rd = rs1 + rs2 (ignore overflow)
	*/
	utils::riscv_instruction_t ADD(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 - rs2 (ignore overflow)
	*/
	utils::riscv_instruction_t SUB(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 << rs2[5:0]
	*/
	utils::riscv_instruction_t SLL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = (rs1 < rs2) ? 1 : 0 (signed)
	*/
	utils::riscv_instruction_t SLT(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = (rs1 < rs2) ? 1 : 0 (unsigned)
	*/
	utils::riscv_instruction_t SLTU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 ⊕ rs2
	*/
	utils::riscv_instruction_t XOR(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 >> rs2[5:0] (logical)
	*/
	utils::riscv_instruction_t SRL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 >> rs2[5:0] (arithmetic)
	*/
	utils::riscv_instruction_t SRA(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 | rs2
	*/
	utils::riscv_instruction_t OR(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 & rs2
	*/
	utils::riscv_instruction_t AND(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	 * system call
	 */
	utils::riscv_instruction_t ECALL();

	// probably not needed: FENCE, FENCE.I, EBREAK, CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI

	/* --- RV64I Base Integer instructions ---------------------------------------------------------------------------- */

	/**
	* rd = (rs1 + rs2) [31:0] sign extended to 64bit
	*/
	utils::riscv_instruction_t ADDW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	 * rd = (rs1 - rs2) [31:0] sign extended to 64bit
	 */
	utils::riscv_instruction_t SUBW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	 * rd = (rs1 << rs2[4:0]) [31:0] sign extended to 64-bit
	 */
	utils::riscv_instruction_t SLLW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	 * rd = (rs1 >> rs2[4:0]) [31:0] sign extended to 64-bit (logical)
	 */
	utils::riscv_instruction_t SRLW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	 * rd = (rs1 >> rs2[4:0]) [31:0] sign extended to 64-bit (arithmetic)
	 */
	utils::riscv_instruction_t SRAW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	 * rd = (rs << shamt) [31:0] sign extended to 64-bit
	 */
	utils::riscv_instruction_t SLLIW(RiscVRegister rd, RiscVRegister rs, uint8_t shamt);

	/**
	 * rd = (rs >> shamt) [31:0] sign extended to 64-bit (logical)
	 */
	utils::riscv_instruction_t SRLIW(RiscVRegister rd, RiscVRegister rs, uint8_t shamt);

	/**
	 * rd = (rs >> shamt) [31:0] sign extended to 64-bit (arithmetic)
	 */
	utils::riscv_instruction_t SRAIW(RiscVRegister rd, RiscVRegister rs, uint8_t shamt);

	/* --- M Standard Extension --------------------------------------------------------------------------------------- */

	/**
	* rd = (rs1 * rs2)[63:0]
	*/
	utils::riscv_instruction_t MUL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = (rs1 * rs2)[127:64] (signed * signed)
	*/
	utils::riscv_instruction_t MULH(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = (rs1 * rs2)[127:64] (signed * unsigned)
	*/
	utils::riscv_instruction_t MULHSU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = (rs1 * rs2)[127:64] (unsigned * unsigned)
	*/
	utils::riscv_instruction_t MULHU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 / rs2 (signed)
	*/
	utils::riscv_instruction_t DIV(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = rs1 / rs2 (unsigned)
	*/
	utils::riscv_instruction_t DIVU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = remainder of rs1 / rs2 (signed)
	*/
	utils::riscv_instruction_t REM(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/**
	* rd = remainder of rs1 / rs2 (unsigned)
	*/
	utils::riscv_instruction_t REMU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2);

	/* --- RV64M Standard Extension --- */
	//*W instructions maybe necessary

	// probably not needed: atomics

	/* --- RV32F Standard Extension ----------------------------------------------------------------------------------- */

	/**
	 * rd = [rs + imm] (single precision)
	 */
	utils::riscv_instruction_t FLW(RiscVFloatingRegister rd, RiscVRegister rs, uint16_t imm);

	/*
	 * [rs1 + imm] = rd (single precision)
	 */
	utils::riscv_instruction_t FSW(RiscVRegister rs1, RiscVFloatingRegister rs2, uint16_t imm);

	/*
	 * rd = rs1*rs2 + rs3 (single precision)
	 */
	utils::riscv_instruction_t FMADDS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3);

	/*
	 * rd = rs1*rs2 + rs3 (single precision)
	 */
	utils::riscv_instruction_t FMSUBS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3);

	/*
	 * rd = -(rs1*rs2 + rs3) (single precision)
	 */
	utils::riscv_instruction_t FNMADDS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3);

	/*
	 * rd = -(rs1*rs2 - rs3) (single precision)
	 */
	utils::riscv_instruction_t FNMSUBS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3);

	/*
	 * rd = rs1 + rs2 (single precision)
	 */
	utils::riscv_instruction_t FADDS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 - rs2 (single precision)
	 */
	utils::riscv_instruction_t FSUBS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 * rs2 (single precision)
	 */
	utils::riscv_instruction_t FMULS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 / rs2 (single precision)
	 */
	utils::riscv_instruction_t FDIVS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = sqrt(s1) (single precision)
	 */
	utils::riscv_instruction_t FSQRTS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1);

	/*
	 * rd = rs1 except the sign, which is taken from rs2 (single precision)
	 */
	utils::riscv_instruction_t FSGNJS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 except the sign, which is the negation of the one from rs2 (single precision)
	 */
	utils::riscv_instruction_t FSGNJNS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 except the sign, which is the XOR of the sign bits of rs1 and rs2 (single precision)
	 */
	utils::riscv_instruction_t FSGNJXS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = min(rs1, rs2) (single precision)
	 */
	utils::riscv_instruction_t FMINS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = max(rs1, rs2) (single precision)
	 */
	utils::riscv_instruction_t FMAXS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * signed 32 bit integer <-- single precision float
	 */
	utils::riscv_instruction_t FCVTWS(RiscVRegister rd, RiscVFloatingRegister rs1);

	/*
	 * unsigned 32 bit integer <-- single precision float
	 */
	utils::riscv_instruction_t FCVTWUS(RiscVRegister rd, RiscVFloatingRegister rs1);

	// FMVXW probably not needed

	/*
	 * rd = rs1 == rs2 (single precision)
	 */
	utils::riscv_instruction_t FEQS(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 < rs2 (single precision)
	 */
	utils::riscv_instruction_t FLTS(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 <= rs2 (single precision)
	 */
	utils::riscv_instruction_t FLES(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	// FCLASS probably not needed

	/*
	 * single precision float <-- signed 32 bit integer
	 */
	utils::riscv_instruction_t FCVTSW(RiscVFloatingRegister rd, RiscVRegister rs1);

	/*
	 * single precision float <-- unsigned 32 bit integer
	 */
	utils::riscv_instruction_t FCVTSWU(RiscVFloatingRegister rd, RiscVRegister rs1);

	/* --- RV64F Standard Extension ----------------------------------------------------------------------------------- */

	/*
	 * signed 64 bit integer <-- single precision float
	 */
	utils::riscv_instruction_t FCVTLS(RiscVRegister rd, RiscVFloatingRegister rs1);

	/*
	 * unsigned 64 bit integer <-- single precision float
	 */
	utils::riscv_instruction_t FCVTLUS(RiscVRegister rd, RiscVFloatingRegister rs1);

	/*
	 * single precision float <-- signed 64 bit integer
	 */
	utils::riscv_instruction_t FCVTSL(RiscVFloatingRegister rd, RiscVRegister rs1);

	/*
	 * single precision float <-- unsigned 64 bit integer
	 */
	utils::riscv_instruction_t FCVTSLU(RiscVFloatingRegister rd, RiscVRegister rs1);

	/* --- RV32D Standard Extension ----------------------------------------------------------------------------------- */

	/**
	 * rd = [rs + imm] (double precision)
	 */
	utils::riscv_instruction_t FLD(RiscVFloatingRegister rd, RiscVRegister rs, uint16_t imm);

	/*
	 * [rs1 + imm] = rd (double precision)
	 */
	utils::riscv_instruction_t FSD(RiscVRegister rs1, RiscVFloatingRegister rs2, uint16_t imm);

	/*
	 * rd = rs1*rs2 + rs3 (double precision)
	 */
	utils::riscv_instruction_t FMADDD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3);

	/*
	 * rd = rs1*rs2 + rs3 (double precision)
	 */
	utils::riscv_instruction_t FMSUBD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3);

	/*
	 * rd = -(rs1*rs2 + rs3) (double precision)
	 */
	utils::riscv_instruction_t FNMADDD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3);

	/*
	 * rd = -(rs1*rs2 - rs3) (double precision)
	 */
	utils::riscv_instruction_t FNMSUBD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3);

	/*
	 * rd = rs1 + rs2 (double precision)
	 */
	utils::riscv_instruction_t FADDD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 - rs2 (double precision)
	 */
	utils::riscv_instruction_t FSUBD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 * rs2 (double precision)
	 */
	utils::riscv_instruction_t FMULD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 / rs2 (double precision)
	 */
	utils::riscv_instruction_t FDIVD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = sqrt(s1) (double precision)
	 */
	utils::riscv_instruction_t FSQRTD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1);

	/*
	 * rd = rs1 except the sign, which is taken from rs2 (double precision)
	 */
	utils::riscv_instruction_t FSGNJD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 except the sign, which is the negation of the one from rs2 (double precision)
	 */
	utils::riscv_instruction_t FSGNJND(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 except the sign, which is the XOR of the sign bits of rs1 and rs2 (double precision)
	 */
	utils::riscv_instruction_t FSGNJXD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = min(rs1, rs2) (double precision)
	 */
	utils::riscv_instruction_t FMIND(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = max(rs1, rs2) (double precision)
	 */
	utils::riscv_instruction_t FMAXD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * single precision float <-- double precision float
	 */
	utils::riscv_instruction_t FCVTSD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1);

	/*
	 * double precision float <-- single precision float
	 */
	utils::riscv_instruction_t FCVTDS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1);

	/*
	 * rd = rs1 == rs2 (double precision)
	 */
	utils::riscv_instruction_t FEQD(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 < rs2 (double precision)
	 */
	utils::riscv_instruction_t FLTD(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	/*
	 * rd = rs1 <= rs2 (double precision)
	 */
	utils::riscv_instruction_t FLED(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2);

	// FCLASS probably not needed

	/*
	 * signed 32 bit integer <-- double precision float
	 */
	utils::riscv_instruction_t FCVTWD(RiscVRegister rd, RiscVFloatingRegister rs1);

	/*
	 * unsigned 32 bit integer <-- double precision float
	 */
	utils::riscv_instruction_t FCVTWUD(RiscVRegister rd, RiscVFloatingRegister rs1);

	/*
	 * double precision float <-- signed 32 bit integer
	 */
	utils::riscv_instruction_t FCVTDW(RiscVFloatingRegister rd, RiscVRegister rs1);

	/*
	 * double precision float <-- unsigned 32 bit integer
	 */
	utils::riscv_instruction_t FCVTDWU(RiscVFloatingRegister rd, RiscVRegister rs1);

	/* --- RV64D Standard Extension ----------------------------------------------------------------------------------- */

	/*
	 * signed 64 bit integer <-- double precision float
	 */
	utils::riscv_instruction_t FCVTLD(RiscVRegister rd, RiscVFloatingRegister rs1);

	/*
	 * unsigned 64 bit integer <-- double precision float
	 */
	utils::riscv_instruction_t FCVTLUD(RiscVRegister rd, RiscVFloatingRegister rs1);

	// FMVXD probably not needed

	/*
	 * double precision float <-- signed 64 bit integer
	 */
	utils::riscv_instruction_t FCVTDL(RiscVFloatingRegister rd, RiscVRegister rs1);

	/*
	 * double precision float <-- unsigned 64 bit integer
	 */
	utils::riscv_instruction_t FCVTDLU(RiscVFloatingRegister rd, RiscVRegister rs1);

	// FMVDX probably not needed

	/* --- Pseudoinstructions ----------------------------------------------------------------------------------------- */

	/**
	*/
	utils::riscv_instruction_t NOP();

	/**
	* rd = rs
	*/
	utils::riscv_instruction_t MV(RiscVRegister rd, RiscVRegister rs);

	/**
	* rd = !rs
	*/
	utils::riscv_instruction_t NOT(RiscVRegister rd, RiscVRegister rs);

	/**
	* rd = -rs
	*/
	utils::riscv_instruction_t NEG(RiscVRegister rd, RiscVRegister rs);

	/**
	* rd = (rs == 0) ? 1 : 0
	*/
	utils::riscv_instruction_t SEQZ(RiscVRegister rd, RiscVRegister rs);

	/**
	* rd == (rs != 0) ? 1 : 0
	*/
	utils::riscv_instruction_t SNEZ(RiscVRegister rd, RiscVRegister rs);

	/**
	* rd == (rs < 0) ? 1 : 0
	*/
	utils::riscv_instruction_t SLTZ(RiscVRegister rd, RiscVRegister rs);

	/**
	* rd == (rs > 0) ? 1 : 0
	*/
	utils::riscv_instruction_t SGTZ(RiscVRegister rd, RiscVRegister rs);

	/**
	 * rd = rs (single precision)
	 */
	utils::riscv_instruction_t FMVS(RiscVFloatingRegister rd, RiscVFloatingRegister rs);

	/**
	 * rd = abs(rs) (single precision)
	 */
	utils::riscv_instruction_t FABSS(RiscVFloatingRegister rd, RiscVFloatingRegister rs);

	/**
	 * rd = -rs (single precision
	 */
	utils::riscv_instruction_t FNEGS(RiscVFloatingRegister rd, RiscVFloatingRegister rs);

	/**
	 * rd = rs (double precision)
	 */
	utils::riscv_instruction_t FMVD(RiscVFloatingRegister rd, RiscVFloatingRegister rs);

	/**
	 * rd = abs(rs) (double precision)
	 */
	utils::riscv_instruction_t FABSD(RiscVFloatingRegister rd, RiscVFloatingRegister rs);

	/**
	 * rd = -rs (double precision)
	 */
	utils::riscv_instruction_t FNEGD(RiscVFloatingRegister rd, RiscVFloatingRegister rs);

	/**
	* pc += (rs == 0) ? offset : 0
	*/
	utils::riscv_instruction_t BEQZ(RiscVRegister rs, uint32_t offset);

	/**
	* pc += (rs != 0) ? offset : 0
	*/
	utils::riscv_instruction_t BNQZ(RiscVRegister rs, uint32_t offset);

	/**
	* pc += (rs <= 0) ? offset : 0
	*/
	utils::riscv_instruction_t BLEZ(RiscVRegister rs, uint32_t offset);

	/**
	* pc += (rs >= 0) ? offset : 0
	*/
	utils::riscv_instruction_t BGEZ(RiscVRegister rs, uint32_t offset);

	/**
	* pc += (rs < 0) ? offset : 0
	*/
	utils::riscv_instruction_t BLTZ(RiscVRegister rs, uint32_t offset);

	/**
	* pc += (rs > 0) ? offset : 0
	*/
	utils::riscv_instruction_t BGTZ(RiscVRegister rs, uint32_t offset);

	/**
	* pc += (rs1 > rs2) ? offset : 0
	*/
	utils::riscv_instruction_t BGT(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* pc += (rs1 <= rs2) ? offset : 0
	*/
	utils::riscv_instruction_t BLE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* pc += (rs1 > rs2) ? offset : 0 (unsigned)
	*/
	utils::riscv_instruction_t BGTU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* pc += (rs1 <= rs2) ? offset : 0 (unsigned)
	*/
	utils::riscv_instruction_t BLEU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset);

	/**
	* unconditional jump
	*/
	utils::riscv_instruction_t J(uint32_t target);
}

#endif
