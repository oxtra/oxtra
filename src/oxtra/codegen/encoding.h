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
		t3, t4, t5, t6, // temporaries
		pc			// program counter
	};

	struct RType {
		uint32_t opcode : 7;
		uint32_t rd : 5;
		uint32_t funct3 : 3;
		uint32_t rs1 : 5;
		uint32_t rs2 : 5;
		uint32_t funct7 : 7;
	};

	struct IType {
		uint32_t opcode : 7;
		uint32_t rd : 5;
		uint32_t funct3 : 3;
		uint32_t rs1 : 5;
		uint32_t imm : 12;
	};

	struct SType {
		uint32_t opcode : 7;
		uint32_t immlow : 5;
		uint32_t funct3 : 3;
		uint32_t rs1 : 5;
		uint32_t rs2 : 5;
		uint32_t immhigh : 7;
	};

	struct UType {
		uint32_t opcode : 7;
		uint32_t rd : 5;
		uint32_t imm : 20;
	};

	struct BType {
		uint32_t opcode : 7;
		uint32_t imm_11 : 1;
		uint32_t imm_4_1 : 4;
		uint32_t funct3 : 3;
		uint32_t rs1 : 5;
		uint32_t rs2 : 5;
		uint32_t imm_10_5 : 6;
		uint32_t imm_12 : 1;
	};

	struct JType {
		uint32_t opcode : 7;
		uint32_t rd : 5;
		uint32_t imm_19_12 : 8;
		uint32_t imm_11 : 1;
		uint32_t imm_10_1 : 10;
		uint32_t imm_20 : 1;
	};

	/* --- Instruction Types --- */

	utils::riscv_instruction_t rtype(uint8_t opcode, uint8_t rd, uint8_t funct3, uint8_t rs1, uint8_t rs2, uint8_t funct7);

	utils::riscv_instruction_t itype(uint8_t opcode, uint8_t rd, uint8_t funct3, uint8_t rs1, uint16_t imm);

	utils::riscv_instruction_t stype(uint8_t opcode, uint8_t funct3, uint8_t rs1, uint8_t rs2, uint16_t imm);

	utils::riscv_instruction_t utype(uint8_t opcode, uint8_t rd, uint32_t imm);

	utils::riscv_instruction_t btype(uint8_t opcode, uint8_t funct3, uint8_t rs1, uint8_t rs2, uint32_t imm);

	utils::riscv_instruction_t jtype(uint8_t opcode, uint8_t rd, uint32_t imm);

	/* --- RV32I Base Instruction Set --- */

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
	* pc += offset[20:1]
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

	/**
	* rd = [rs1 + imm] (64 bit)
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

	utils::riscv_instruction_t probably not needed: FENCE, FENCE.I, ECALL, EBREAK, CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI

	/* --- RV64I Base Integer Instructions --- */
	utils::riscv_instruction_t *W instructions maybe necessary

	utils::riscv_instruction_t --- M Standard Extension --- */

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
	utils::riscv_instruction_t *W instructions maybe necessary

	// probably not needed: atomics
	utils::riscv_instruction_t nice to have: floating point

	utils::riscv_instruction_t --- Pseudoinstructions --- */

	/**
	*/
	utils::riscv_instruction_t NOP();

	/**
	* rd = imm
	*/
	utils::riscv_instruction_t LI(RiscVRegister rd, uint32_t imm);

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
