#include <cstdint>
#include "encoding.h"

/* --- Instruction Types --- */

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

riscv_instruction_t LUI(RVREG rd, uint32_t imm) {
	return utype(55, rd, imm);
}

riscv_instruction_t AUIPC(RVREG rd, uint32_t imm) {
	return utype(23, rd, imm);
}

riscv_instruction_t JAL(RVREG rd, uint32_t offset) {
	return jtype(103, rd, offset);
}

riscv_instruction_t JALR(RVREG rd, RVREG rs1, uint16_t offset) {
	return itype(103, rd, 0, rs1, offset);
}

riscv_instruction_t BEQ(RVREG rs1, RVREG rs2, uint32_t offset) {
	return btype(99, 0, rs1, rs2, offset);
}

riscv_instruction_t BNE(RVREG rs1, RVREG rs2, uint32_t offset) {
	return btype(99, 1, rs1, rs2, offset);
}

riscv_instruction_t BLT(RVREG rs1, RVREG rs2, uint32_t offset) {
	return btype(99, 4, rs1, rs2, offset);
}

riscv_instruction_t BGE(RVREG rs1, RVREG rs2, uint32_t offset) {
	return btype(99, 5, rs1, rs2, offset);
}

riscv_instruction_t BLTU(RVREG rs1, RVREG rs2, uint32_t offset) {
	return btype(99, 6, rs1, rs2, offset);
}

riscv_instruction_t BGEU(RVREG rs1, RVREG rs2, uint32_t offset) {
	return btype(99, 7, rs1, rs2, offset);
}

riscv_instruction_t LB(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(3, rd, 0, rs1, imm);
}

riscv_instruction_t LH(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(3, rd, 1, rs1, imm);
}

riscv_instruction_t LW(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(3, rd, 2, rs1, imm);
}

riscv_instruction_t LD(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(3, rd, 3, rs1, imm);
}

riscv_instruction_t LBU(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(3, rd, 4, rs1, imm);
}

riscv_instruction_t LHU(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(3, rd, 5, rs1, imm);
}

riscv_instruction_t LWU(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(3, rd, 6, rs1, imm);
}

riscv_instruction_t SB(RVREG rs1, RVREG rs2, uint32_t imm) {
	return stype(35, 0, rs1, rs2, imm);
}

riscv_instruction_t SH(RVREG rs1, RVREG rs2, uint32_t imm) {
	return stype(35, 1, rs1, rs2, imm);
}

riscv_instruction_t SW(RVREG rs1, RVREG rs2, uint32_t imm) {
	return stype(35, 2, rs1, rs2, imm);
}

riscv_instruction_t SD(RVREG rs1, RVREG rs2, uint32_t imm) {
	return stype(35, 3, rs1, rs2, imm);
}

riscv_instruction_t ADDI(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(19, rd, 0, rs1, imm);
}

riscv_instruction_t SLTI(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(19, rd, 1, rs1, imm);
}

riscv_instruction_t SLTIU(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(19, rd, 2, rs1, imm);
}

riscv_instruction_t XORI(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(19, rd, 3, rs1, imm);
}

riscv_instruction_t ORI(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(19, rd, 4, rs1, imm);
}

riscv_instruction_t ANDI(RVREG rd, RVREG rs1, uint16_t imm) {
	return itype(19, rd, 5, rs1, imm);
}

riscv_instruction_t SLLI(RVREG rd, RVREG rs1, uint8_t shamt) {
	return itype(19, rd, 1, rs1, shamt & 0x3f);
}

riscv_instruction_t SRLI(RVREG rd, RVREG rs1, uint8_t shamt) {
	return itype(19, rd, 5, rs1, shamt & 0x3f);
}

riscv_instruction_t SRAI(RVREG rd, RVREG rs1, uint8_t shamt) {
	return itype(19, rd, 5, rs1, (shamt & 0x3f) | (1 << 10));
}

riscv_instruction_t ADD(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 0, rs1, rs2, 0);
}

riscv_instruction_t SUB(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 0, rs1, rs2, 32);
}

riscv_instruction_t SLL(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 1, rs1, rs2, 0);
}

riscv_instruction_t SLT(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 2, rs1, rs2, 0);
}

riscv_instruction_t SLTU(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 3, rs1, rs2, 0);
}

riscv_instruction_t XOR(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 4, rs1, rs2, 0);
}

riscv_instruction_t SRL(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 5, rs1, rs2, 0);
}

riscv_instruction_t SRA(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 5, rs1, rs2, 32);
}

riscv_instruction_t OR(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 6, rs1, rs2, 0);
}

riscv_instruction_t AND(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 7, rs1, rs2, 0);
}

riscv_instruction_t probably not needed: FENCE, FENCE.I, ECALL, EBREAK, CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI

riscv_instruction_t --- M Standard Extensions --- */

riscv_instruction_t MUL(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 0, rs1, rs2, 1);
}

riscv_instruction_t MULH(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 1, rs1, rs2, 1);
}

riscv_instruction_t MULHSU(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 2, rs1, rs2, 1);
}

riscv_instruction_t MULHU(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 3, rs1, rs2, 1);
}

riscv_instruction_t DIV(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 4, rs1, rs2, 1);
}

riscv_instruction_t DIVU(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 5, rs1, rs2, 1);
}

riscv_instruction_t REM(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 6, rs1, rs2, 1);
}

riscv_instruction_t REMU(RVREG rd, RVREG rs1, RVREG rs2) {
	return rtype(51, rd, 7, rs1, rs2, 1);
}

riscv_instruction_t maybe needed: *W instructions

riscv_instruction_t --- Pseudoinstructions --- */

riscv_instruction_t NOP() {
	return ADDI(zero, zero, 0);
}

riscv_instruction_t LI(RVREG rd, uint32_t imm) {
	// more than one instruction
}

riscv_instruction_t MV(RVREG rd, RVREG rs) {
	return ADDI(rd, rs, 0);
}

riscv_instruction_t NOT(RVREG rd, RVREG rs) {
	return XORI(rd, rs, -1);
}

riscv_instruction_t NEG(RVREG rd, RVREG rs) {
	return SUB(rd, zero, rs);
}

riscv_instruction_t SEQZ(RVREG rd, RVREG rs) {
	return SLTIU(rd, rs, 1);
}

riscv_instruction_t SNEZ(RVREG rd, RVREG rs) {
	return SLTU(rd, zero, rd);
}

riscv_instruction_t SLTZ(RVREG rd, RVREG rs) {
	return SLT(rd, rs, zero);
}

riscv_instruction_t SGTZ(RVREG rd, RVREG rs) {
	return SLT(rd, zero, rs);
}

riscv_instruction_t BEQZ(RVREG rs, uint32_t offset) {
	return BEQ(rs, zero, offset);
}

riscv_instruction_t BNQZ(RVREG rs, uint32_t offset) {
	return BNE(rs, zero, offset);
}

riscv_instruction_t BLEZ(RVREG rs, uint32_t offset) {
	return BGE(zero, rs, offset);
}

riscv_instruction_t BGEZ(RVREG rs, uint32_t offset) {
	return BGE(rs, zero, offset);
}

riscv_instruction_t BLTZ(RVREG rs, uint32_t offset) {
	return BLT(rs, zero, offset);
}

riscv_instruction_t BGTZ(RVREG rs, uint32_t offset) {
	return BEQ(zero, rs, offset);
}

riscv_instruction_t BGT(RVREG rs1, RVREG rs2, uint32_t offset) {
	return BLT(rs2, rs1, offset);
}

riscv_instruction_t BLE(RVREG rs1, RVREG rs2, uint32_t offset) {
	return BGE(rs2, rs1, offset);
}

riscv_instruction_t BGTU(RVREG rs1, RVREG rs2, uint32_t offset) {
	return BLTU(rs2, rs1, offset);
}

riscv_instruction_t BLEU(RVREG rs1, RVREG rs2, uint32_t offset) {
	return BGEU(rs2, rs1, offset);
}

riscv_instruction_t J(uint32_t offset) {
	return JAL(zero, offset);
}
