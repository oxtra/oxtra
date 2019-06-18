#include <cstdint>
#include "encoding.h"

/* --- Instruction Types --- */

using namespace utils;

riscv_instruction_t rtype(uint8_t opcode, uint8_t rd, uint8_t funct3, uint8_t rs1, uint8_t rs2, uint8_t funct7) {
	uint32_t rtype = (funct7 << 25) + (rs2 << 20) + (rs1 << 15) + (funct3 << 12) + (rd << 7) + opcode;
	return rtype;
};

riscv_instruction_t itype(uint8_t opcode, uint8_t rd, uint8_t funct3, uint8_t rs1, uint16_t imm) {
	uint32_t itype = (imm << 20) + (rs1 << 15) + (funct3 <<12) + (rd << 7) + opcode;
	return itype;
}

riscv_instruction_t stype(uint8_t opcode, uint8_t funct3, uint8_t rs1, uint8_t rs2, uint16_t imm) {
	uint8_t immlow = imm & 0x1f;
	uint8_t immhigh = (imm >> 5) & 0x7f;
	uint32_t stype  = (immhigh << 25) + (rs2 << 20) + (rs1 << 15) + (funct3 << 12) + (immlow << 7) + opcode;
	return stype;
}

riscv_instruction_t utype(uint8_t opcode, uint8_t rd, uint32_t imm) {
	uint32_t utype = (imm << 12) + (rd << 7) + opcode;
	return utype;
}

riscv_instruction_t btype(uint8_t opcode, uint8_t funct3, uint8_t rs1, uint8_t rs2, uint32_t imm) {
	uint8_t imm_11 = (imm >> 11) & 1;
	uint8_t imm_4_1 = (imm >> 1) & 0xf;
	uint8_t imm_10_5 = (imm >> 5) & 0x3f;
	uint8_t imm_12 = (imm >> 12) & 1;
	uint32_t btype = (imm_12 << 31) + (imm_10_5 << 25) + (rs2 << 20) + (rs1 << 15) + (funct3 << 12) + (imm_4_1 << 8) + (imm_11 << 7) + opcode;
	return btype;
}

riscv_instruction_t jtype(uint8_t opcode, uint8_t rd, uint32_t imm) {
	uint8_t imm_20 = (imm >> 20) & 1;
	uint16_t imm_10_1 = (imm >> 1) & 0x3ff;
	uint8_t imm_11 = (imm >> 11) & 1;
	uint8_t imm_19_12 = (imm > 12) & 0xff;
	uint32_t jtype = (imm_20 << 31) + (imm_10_1 << 21) + (imm_11 << 20) + (imm_19_12 << 12) + (rd << 7) + opcode;
	return jtype;
}

/* --- RV32I Base Instruction Set --- */

riscv_instruction_t LUI(RiscVRegister rd, uint32_t imm) {
	return utype(55, rd, imm);
}

riscv_instruction_t AUIPC(RiscVRegister rd, uint32_t imm) {
	return utype(23, rd, imm);
}

riscv_instruction_t JAL(RiscVRegister rd, uint32_t offset) {
	return jtype(103, rd, offset);
}

riscv_instruction_t JALR(RiscVRegister rd, RiscVRegister rs1, uint16_t offset) {
	return itype(103, rd, 0, rs1, offset);
}

riscv_instruction_t BEQ(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 0, rs1, rs2, offset);
}

riscv_instruction_t BNE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 1, rs1, rs2, offset);
}

riscv_instruction_t BLT(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 4, rs1, rs2, offset);
}

riscv_instruction_t BGE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 5, rs1, rs2, offset);
}

riscv_instruction_t BLTU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 6, rs1, rs2, offset);
}

riscv_instruction_t BGEU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 7, rs1, rs2, offset);
}

riscv_instruction_t LB(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 0, rs1, imm);
}

riscv_instruction_t LH(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 1, rs1, imm);
}

riscv_instruction_t LW(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 2, rs1, imm);
}

riscv_instruction_t LD(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 3, rs1, imm);
}

riscv_instruction_t LBU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 4, rs1, imm);
}

riscv_instruction_t LHU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 5, rs1, imm);
}

riscv_instruction_t LWU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 6, rs1, imm);
}

riscv_instruction_t SB(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	return stype(35, 0, rs1, rs2, imm);
}

riscv_instruction_t SH(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	return stype(35, 1, rs1, rs2, imm);
}

riscv_instruction_t SW(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	return stype(35, 2, rs1, rs2, imm);
}

riscv_instruction_t SD(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	return stype(35, 3, rs1, rs2, imm);
}

riscv_instruction_t ADDI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 0, rs1, imm);
}

riscv_instruction_t SLTI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 1, rs1, imm);
}

riscv_instruction_t SLTIU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 2, rs1, imm);
}

riscv_instruction_t XORI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 3, rs1, imm);
}

riscv_instruction_t ORI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 4, rs1, imm);
}

riscv_instruction_t ANDI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 5, rs1, imm);
}

riscv_instruction_t SLLI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt) {
	return itype(19, rd, 1, rs1, shamt & 0x3f);
}

riscv_instruction_t SRLI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt) {
	return itype(19, rd, 5, rs1, shamt & 0x3f);
}

riscv_instruction_t SRAI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt) {
	return itype(19, rd, 5, rs1, (shamt & 0x3f) | (1 << 10));
}

riscv_instruction_t ADD(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 0, rs1, rs2, 0);
}

riscv_instruction_t SUB(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 0, rs1, rs2, 32);
}

riscv_instruction_t SLL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 1, rs1, rs2, 0);
}

riscv_instruction_t SLT(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 2, rs1, rs2, 0);
}

riscv_instruction_t SLTU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 3, rs1, rs2, 0);
}

riscv_instruction_t XOR(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 4, rs1, rs2, 0);
}

riscv_instruction_t SRL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 5, rs1, rs2, 0);
}

riscv_instruction_t SRA(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 5, rs1, rs2, 32);
}

riscv_instruction_t OR(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 6, rs1, rs2, 0);
}

riscv_instruction_t AND(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 7, rs1, rs2, 0);
}

riscv_instruction_t probably not needed: FENCE, FENCE.I, ECALL, EBREAK, CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI

riscv_instruction_t --- M Standard Extensions --- */

riscv_instruction_t MUL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 0, rs1, rs2, 1);
}

riscv_instruction_t MULH(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 1, rs1, rs2, 1);
}

riscv_instruction_t MULHSU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 2, rs1, rs2, 1);
}

riscv_instruction_t MULHU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 3, rs1, rs2, 1);
}

riscv_instruction_t DIV(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 4, rs1, rs2, 1);
}

riscv_instruction_t DIVU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 5, rs1, rs2, 1);
}

riscv_instruction_t REM(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 6, rs1, rs2, 1);
}

riscv_instruction_t REMU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 7, rs1, rs2, 1);
}

riscv_instruction_t maybe needed: *W instructions

riscv_instruction_t --- Pseudoinstructions --- */

riscv_instruction_t NOP() {
	return ADDI(zero, zero, 0);
}

riscv_instruction_t LI(RiscVRegister rd, uint32_t imm) {
	// more than one instruction
}

riscv_instruction_t MV(RiscVRegister rd, RiscVRegister rs) {
	return ADDI(rd, rs, 0);
}

riscv_instruction_t NOT(RiscVRegister rd, RiscVRegister rs) {
	return XORI(rd, rs, -1);
}

riscv_instruction_t NEG(RiscVRegister rd, RiscVRegister rs) {
	return SUB(rd, zero, rs);
}

riscv_instruction_t SEQZ(RiscVRegister rd, RiscVRegister rs) {
	return SLTIU(rd, rs, 1);
}

riscv_instruction_t SNEZ(RiscVRegister rd, RiscVRegister rs) {
	return SLTU(rd, zero, rd);
}

riscv_instruction_t SLTZ(RiscVRegister rd, RiscVRegister rs) {
	return SLT(rd, rs, zero);
}

riscv_instruction_t SGTZ(RiscVRegister rd, RiscVRegister rs) {
	return SLT(rd, zero, rs);
}

riscv_instruction_t BEQZ(RiscVRegister rs, uint32_t offset) {
	return BEQ(rs, zero, offset);
}

riscv_instruction_t BNQZ(RiscVRegister rs, uint32_t offset) {
	return BNE(rs, zero, offset);
}

riscv_instruction_t BLEZ(RiscVRegister rs, uint32_t offset) {
	return BGE(zero, rs, offset);
}

riscv_instruction_t BGEZ(RiscVRegister rs, uint32_t offset) {
	return BGE(rs, zero, offset);
}

riscv_instruction_t BLTZ(RiscVRegister rs, uint32_t offset) {
	return BLT(rs, zero, offset);
}

riscv_instruction_t BGTZ(RiscVRegister rs, uint32_t offset) {
	return BEQ(zero, rs, offset);
}

riscv_instruction_t BGT(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return BLT(rs2, rs1, offset);
}

riscv_instruction_t BLE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return BGE(rs2, rs1, offset);
}

riscv_instruction_t BGTU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return BLTU(rs2, rs1, offset);
}

riscv_instruction_t BLEU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return BGEU(rs2, rs1, offset);
}

riscv_instruction_t J(uint32_t offset) {
	return JAL(zero, offset);
}
