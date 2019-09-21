#include <cstdint>
#include "encoding.h"

/* --- Instruction Types --- */

using namespace utils;
using namespace encoding;

uint32_t rtype(uint8_t opcode, RiscVRegister rd, uint8_t funct3, RiscVRegister rs1, RiscVRegister rs2, uint8_t funct7) {
	RType rtype;
	rtype.opcode = opcode;
	rtype.rd = static_cast<uint8_t>(rd);
	rtype.funct3 = funct3;
	rtype.rs1 = static_cast<uint8_t>(rs1);
	rtype.rs2 = static_cast<uint8_t>(rs2);
	rtype.funct7 = funct7;
	return rtype.raw;
};

uint32_t itype(uint8_t opcode, RiscVRegister rd, uint8_t funct3, RiscVRegister rs1, uint16_t imm) {
	IType itype;
	itype.opcode = opcode;
	itype.rd = static_cast<uint8_t>(rd);
	itype.funct3 = funct3;
	itype.rs1 = static_cast<uint8_t>(rs1);
	itype.imm = static_cast<uint32_t>(imm);
	return itype.raw;
}

uint32_t stype(uint8_t opcode, uint8_t funct3, RiscVRegister rs1, RiscVRegister rs2, uint16_t imm) {
	SType stype;
	stype.opcode = opcode;
	stype.immlow = static_cast<uint32_t>(imm) & 0x1fu;
	stype.funct3 = funct3;
	stype.rs1 = static_cast<uint8_t>(rs1);
	stype.rs2 = static_cast<uint8_t>(rs2);
	stype.immhigh = static_cast<uint32_t>(imm >> 5u) & 0x7fu;
	return stype.raw;
}

uint32_t utype(uint8_t opcode, RiscVRegister rd, uint32_t imm) {
	UType utype;
	utype.opcode = opcode;
	utype.rd = static_cast<uint8_t>(rd);
	utype.imm = imm;
	return utype.raw;
}

uint32_t btype(uint8_t opcode, uint8_t funct3, RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	BType btype;
	btype.opcode = opcode;
	btype.imm_11 = (imm >> 11) & 1;
	btype.imm_4_1 = (imm >> 1) & 0xf;
	btype.funct3 = funct3;
	btype.rs1 = static_cast<uint8_t>(rs1);
	btype.rs2 = static_cast<uint8_t>(rs2);
	btype.imm_10_5 = (imm >> 5) & 0x3f;
	btype.imm_12 = (imm >> 12) & 1;
	return btype.raw;
}

uint32_t jtype(uint8_t opcode, RiscVRegister rd, uint32_t imm) {
	JType jtype;
	jtype.opcode = opcode;
	jtype.rd = static_cast<uint8_t>(rd);
	jtype.imm_19_12 = (imm >> 12) & 0xff;
	jtype.imm_11 = (imm >> 11) & 1;
	jtype.imm_10_1 = (imm >> 1) & 0x3ff;
	jtype.imm_20 = (imm >> 20) & 1;
	return jtype.raw;
}

uint32_t frtype(uint8_t opcode, RiscVFloatingRegister rd, uint8_t funct3, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, uint8_t funct7) {
	RType rtype;
	rtype.opcode = opcode;
	rtype.rd = static_cast<uint8_t>(rd);
	rtype.funct3 = funct3;
	rtype.rs1 = static_cast<uint8_t>(rs1);
	rtype.rs2 = static_cast<uint8_t>(rs2);
	rtype.funct7 = funct7;
	return rtype.raw;
};

uint32_t r4type(uint8_t opcode, RiscVFloatingRegister rd, uint8_t funct3, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, uint8_t funct2, RiscVFloatingRegister rs3) {
	R4Type r4type;
	r4type.opcode = opcode;
	r4type.rd = static_cast<uint8_t>(rd);
	r4type.funct3 = funct3;
	r4type.rs1 = static_cast<uint8_t>(rs1);
	r4type.rs2 = static_cast<uint8_t>(rs2);
	r4type.funct2 = funct2;
	r4type.rs3 = static_cast<uint8_t>(rs3);
	return r4type.raw;
};

/* --- RV32I Base Instruction Set ------------------------------------------------------------------------------------- */

riscv_instruction_t encoding::LUI(RiscVRegister rd, uint32_t imm) {
	return utype(55, rd, imm);
}

riscv_instruction_t encoding::AUIPC(RiscVRegister rd, uint32_t imm) {
	return utype(23, rd, imm);
}

riscv_instruction_t encoding::JAL(RiscVRegister rd, uint32_t offset) {
	return jtype(111, rd, offset);
}

riscv_instruction_t encoding::JALR(RiscVRegister rd, RiscVRegister rs1, uint16_t offset) {
	return itype(103, rd, 0, rs1, offset);
}

riscv_instruction_t encoding::BEQ(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 0, rs1, rs2, offset);
}

riscv_instruction_t encoding::BNE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 1, rs1, rs2, offset);
}

riscv_instruction_t encoding::BLT(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 4, rs1, rs2, offset);
}

riscv_instruction_t encoding::BGE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 5, rs1, rs2, offset);
}

riscv_instruction_t encoding::BLTU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 6, rs1, rs2, offset);
}

riscv_instruction_t encoding::BGEU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return btype(99, 7, rs1, rs2, offset);
}

riscv_instruction_t encoding::LB(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 0, rs1, imm);
}

riscv_instruction_t encoding::LH(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 1, rs1, imm);
}

riscv_instruction_t encoding::LW(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 2, rs1, imm);
}

riscv_instruction_t encoding::LD(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 3, rs1, imm);
}

riscv_instruction_t encoding::LBU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 4, rs1, imm);
}

riscv_instruction_t encoding::LHU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 5, rs1, imm);
}

riscv_instruction_t encoding::LWU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(3, rd, 6, rs1, imm);
}

riscv_instruction_t encoding::SB(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	return stype(35, 0, rs1, rs2, imm);
}

riscv_instruction_t encoding::SH(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	return stype(35, 1, rs1, rs2, imm);
}

riscv_instruction_t encoding::SW(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	return stype(35, 2, rs1, rs2, imm);
}

riscv_instruction_t encoding::SD(RiscVRegister rs1, RiscVRegister rs2, uint32_t imm) {
	return stype(35, 3, rs1, rs2, imm);
}

riscv_instruction_t encoding::ADDI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 0, rs1, imm);
}

riscv_instruction_t encoding::SLTI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 2, rs1, imm);
}

riscv_instruction_t encoding::SLTIU(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 3, rs1, imm);
}

riscv_instruction_t encoding::XORI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 4, rs1, imm);
}

riscv_instruction_t encoding::ORI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 6, rs1, imm);
}

riscv_instruction_t encoding::ANDI(RiscVRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(19, rd, 7, rs1, imm);
}

riscv_instruction_t encoding::SLLI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt) {
	return itype(19, rd, 1, rs1, shamt & 0x3f);
}

riscv_instruction_t encoding::SRLI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt) {
	return itype(19, rd, 5, rs1, shamt & 0x3f);
}

riscv_instruction_t encoding::SRAI(RiscVRegister rd, RiscVRegister rs1, uint8_t shamt) {
	return itype(19, rd, 5, rs1, (shamt & 0x3f) | (1 << 10));
}

riscv_instruction_t encoding::ADD(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 0, rs1, rs2, 0);
}

riscv_instruction_t encoding::SUB(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 0, rs1, rs2, 32);
}

riscv_instruction_t encoding::SLL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 1, rs1, rs2, 0);
}

riscv_instruction_t encoding::SLT(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 2, rs1, rs2, 0);
}

riscv_instruction_t encoding::SLTU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 3, rs1, rs2, 0);
}

riscv_instruction_t encoding::XOR(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 4, rs1, rs2, 0);
}

riscv_instruction_t encoding::SRL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 5, rs1, rs2, 0);
}

riscv_instruction_t encoding::SRA(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 5, rs1, rs2, 32);
}

riscv_instruction_t encoding::OR(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 6, rs1, rs2, 0);
}

riscv_instruction_t encoding::AND(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 7, rs1, rs2, 0);
}

riscv_instruction_t encoding::ECALL() {
	return itype(115, RiscVRegister::zero, 0, RiscVRegister::zero, 0);
}

/* --- RV64I Base Instruction Set ------------------------------------------------------------------------------------- */

riscv_instruction_t encoding::ADDW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(59, rd, 0, rs1, rs2, 0);
}

riscv_instruction_t encoding::SUBW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(59, rd, 0, rs1, rs2, 32);
}

riscv_instruction_t encoding::SLLW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(59, rd, 1, rs1, rs2, 0);
}

riscv_instruction_t encoding::SRLW(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(59, rd, 5, rs1, rs2, 0);
}

// probably not needed: FENCE, FENCE.I, EBREAK, CSRRW, CSRRS, CSRRC, CSRRWI, CSRRSI, CSRRCI

/* --- M Standard Extensions ------------------------------------------------------------------------------------------ */

riscv_instruction_t encoding::MUL(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 0, rs1, rs2, 1);
}

riscv_instruction_t encoding::MULH(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 1, rs1, rs2, 1);
}

riscv_instruction_t encoding::MULHSU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 2, rs1, rs2, 1);
}

riscv_instruction_t encoding::MULHU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 3, rs1, rs2, 1);
}

riscv_instruction_t encoding::DIV(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 4, rs1, rs2, 1);
}

riscv_instruction_t encoding::DIVU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 5, rs1, rs2, 1);
}

riscv_instruction_t encoding::REM(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 6, rs1, rs2, 1);
}

riscv_instruction_t encoding::REMU(RiscVRegister rd, RiscVRegister rs1, RiscVRegister rs2) {
	return rtype(51, rd, 7, rs1, rs2, 1);
}

// maybe needed: *W instructions

/* --- RV32F Standard Extension --------------------------------------------------------------------------------------- */

utils::riscv_instruction_t encoding::FLW(RiscVFloatingRegister rd, RiscVRegister rs1, uint16_t imm) {
	return itype(7, static_cast<RiscVRegister>(rd), 2, rs1, imm);
}

utils::riscv_instruction_t encoding::FSW(RiscVRegister rs1, RiscVFloatingRegister rs2, uint16_t imm) {
	return stype(39, 2, rs1, static_cast<RiscVRegister>(rs2), imm);
}

utils::riscv_instruction_t encoding::FMADDS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3) {
	return r4type(67, rd, 0, rs1, rs2, 0, rs3);
}

utils::riscv_instruction_t encoding::FNMSUBS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3) {
	return r4type(71, rd, 0, rs1, rs2, 0, rs3);
}

utils::riscv_instruction_t encoding::FMSUBS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3) {
	return r4type(75, rd, 0, rs1, rs2, 0, rs3);
}

utils::riscv_instruction_t encoding::FNMADDS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3) {
	return r4type(79, rd, 0, rs1, rs2, 0, rs3);
}

utils::riscv_instruction_t encoding::FADDS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 0);
}

utils::riscv_instruction_t encoding::FSUBS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 4);
}

utils::riscv_instruction_t encoding::FMULS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 8);
}

utils::riscv_instruction_t encoding::FDIVS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 12);
}

utils::riscv_instruction_t encoding::FSQRTS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, rd, 0, rs1, static_cast<RiscVFloatingRegister>(0), 44);
}

utils::riscv_instruction_t encoding::FSGNJS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 16);
}

utils::riscv_instruction_t encoding::FSGNJNS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 1, rs1, rs2, 16);
}

utils::riscv_instruction_t encoding::FSGNJXS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 2, rs1, rs2, 16);
}

utils::riscv_instruction_t encoding::FMINS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 20);
}

utils::riscv_instruction_t encoding::FMAXS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 1, rs1, rs2, 20);
}

utils::riscv_instruction_t encoding::FCVTWS(RiscVRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, static_cast<RiscVFloatingRegister>(0), 96);
}

utils::riscv_instruction_t encoding::FCVTWUS(RiscVRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, static_cast<RiscVFloatingRegister>(1), 96);
}

// FMVXW probably not needed

utils::riscv_instruction_t encoding::FEQS(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 2, rs1, rs2, 70);
}

utils::riscv_instruction_t encoding::FLTS(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 1, rs1, rs2, 70);
}

utils::riscv_instruction_t encoding::FLES(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, rs2, 70);
}

// FCLASS probably not needed

utils::riscv_instruction_t encoding::FCVTSW(RiscVFloatingRegister rd, RiscVRegister rs1) {
	return frtype(83, rd, 0, static_cast<RiscVFloatingRegister>(rs1), static_cast<RiscVFloatingRegister>(0), 112);
}

utils::riscv_instruction_t encoding::FCVTSWU(RiscVFloatingRegister rd, RiscVRegister rs1) {
	return frtype(83, rd, 0, static_cast<RiscVFloatingRegister>(rs1), static_cast<RiscVFloatingRegister>(1), 112);
}

/* --- RV64F Standard Extension --------------------------------------------------------------------------------------- */

utils::riscv_instruction_t encoding::FCVTLS(RiscVRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, static_cast<RiscVFloatingRegister>(2), 96);
}

utils::riscv_instruction_t encoding::FCVTLUS(RiscVRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, static_cast<RiscVFloatingRegister>(3), 96);
}

utils::riscv_instruction_t encoding::FCVTSL(RiscVFloatingRegister rd, RiscVRegister rs1) {
	return frtype(83, rd, 0, static_cast<RiscVFloatingRegister>(rs1), static_cast<RiscVFloatingRegister>(2), 96);
}

utils::riscv_instruction_t encoding::FCVTSLU(RiscVFloatingRegister rd, RiscVRegister rs1) {
	return frtype(83, rd, 0, static_cast<RiscVFloatingRegister>(rs1), static_cast<RiscVFloatingRegister>(3), 96);
}

/* --- RV32D Standard Extension --------------------------------------------------------------------------------------- */

utils::riscv_instruction_t encoding::FLD(RiscVFloatingRegister rd, RiscVRegister rs, uint16_t imm) {
	return itype(7, static_cast<RiscVRegister>(rd), 3, rs, imm);
}

utils::riscv_instruction_t encoding::FSD(RiscVRegister rs1, RiscVFloatingRegister rs2, uint16_t imm) {
	return stype(39, 3, rs1, static_cast<RiscVRegister>(rs2), imm);
}

utils::riscv_instruction_t encoding::FMADDD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3) {
	return r4type(67, rd, 0, rs1, rs2, 1, rs3);
}

utils::riscv_instruction_t encoding::FMSUBD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3) {
	return r4type(71, rd, 0, rs1, rs2, 1, rs3);
}

utils::riscv_instruction_t encoding::FNMADDD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3) {
	return r4type(75, rd, 0, rs1, rs2, 1, rs3);
}

utils::riscv_instruction_t encoding::FNMSUBD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2, RiscVFloatingRegister rs3) {
	return r4type(79, rd, 0, rs1, rs2, 1, rs3);
}

utils::riscv_instruction_t encoding::FADDD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 1);
}

utils::riscv_instruction_t encoding::FSUBD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 5);
}

utils::riscv_instruction_t encoding::FMULD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 9);
}

utils::riscv_instruction_t encoding::FDIVD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 13);
}

utils::riscv_instruction_t encoding::FSQRTD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, rd, 0, rs1, static_cast<RiscVFloatingRegister>(0), 45);
}

utils::riscv_instruction_t encoding::FSGNJD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 17);
}

utils::riscv_instruction_t encoding::FSGNJND(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 1, rs1, rs2, 17);
}

utils::riscv_instruction_t encoding::FSGNJXD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 2, rs1, rs2, 17);
}

utils::riscv_instruction_t encoding::FMIND(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 0, rs1, rs2, 21);
}

utils::riscv_instruction_t encoding::FMAXD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, rd, 1, rs1, rs2, 21);
}

utils::riscv_instruction_t encoding::FCVTSD(RiscVFloatingRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, rd, 0, rs1, static_cast<RiscVFloatingRegister>(1), 32);
}

utils::riscv_instruction_t encoding::FCVTDS(RiscVFloatingRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, rd, 0, rs1, static_cast<RiscVFloatingRegister>(0), 33);
}

utils::riscv_instruction_t encoding::FEQD(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 2, rs1, rs2, 81);
}

utils::riscv_instruction_t encoding::FLTD(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 1, rs1, rs2, 81);
}

utils::riscv_instruction_t encoding::FLED(RiscVRegister rd, RiscVFloatingRegister rs1, RiscVFloatingRegister rs2) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, rs2, 81);
}

// FCLASS probably not needed

utils::riscv_instruction_t encoding::FCVTWD(RiscVRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, static_cast<RiscVFloatingRegister>(0), 97);
}

utils::riscv_instruction_t encoding::FCVTWUD(RiscVRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, static_cast<RiscVFloatingRegister>(1), 97);
}

utils::riscv_instruction_t encoding::FCVTDW(RiscVFloatingRegister rd, RiscVRegister rs1) {
	return frtype(83, rd, 0, static_cast<RiscVFloatingRegister>(rs1), static_cast<RiscVFloatingRegister>(0), 105);
}

utils::riscv_instruction_t encoding::FCVTDWU(RiscVFloatingRegister rd, RiscVRegister rs1) {
	return frtype(83, rd, 0, static_cast<RiscVFloatingRegister>(rs1), static_cast<RiscVFloatingRegister>(1), 105);
}

/* --- RV64D Standard Extension --------------------------------------------------------------------------------------- */

utils::riscv_instruction_t encoding::FCVTLD(RiscVRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, static_cast<RiscVFloatingRegister>(2), 97);
}

utils::riscv_instruction_t encoding::FCVTLUD(RiscVRegister rd, RiscVFloatingRegister rs1) {
	return frtype(83, static_cast<RiscVFloatingRegister>(rd), 0, rs1, static_cast<RiscVFloatingRegister>(3), 97);
}

// FMVXD probably not needed

utils::riscv_instruction_t encoding::FCVTDL(RiscVFloatingRegister rd, RiscVRegister rs1) {
	return frtype(83, rd, 0, static_cast<RiscVFloatingRegister>(rs1), static_cast<RiscVFloatingRegister>(2), 105);
}

utils::riscv_instruction_t encoding::FCVTDLU(RiscVFloatingRegister rd, RiscVRegister rs1) {
	return frtype(83, rd, 0, static_cast<RiscVFloatingRegister>(rs1), static_cast<RiscVFloatingRegister>(3), 105);
}

// FMVDX probably not needed

/* --- Pseudoinstructions --------------------------------------------------------------------------------------------- */

riscv_instruction_t encoding::NOP() {
	return ADDI(RiscVRegister::zero, RiscVRegister::zero, 0);
}

riscv_instruction_t encoding::MV(RiscVRegister rd, RiscVRegister rs) {
	return ADDI(rd, rs, 0);
}

riscv_instruction_t encoding::NOT(RiscVRegister rd, RiscVRegister rs) {
	return XORI(rd, rs, -1);
}

riscv_instruction_t encoding::NEG(RiscVRegister rd, RiscVRegister rs) {
	return SUB(rd, RiscVRegister::zero, rs);
}

riscv_instruction_t encoding::SEQZ(RiscVRegister rd, RiscVRegister rs) {
	return SLTIU(rd, rs, 1);
}

riscv_instruction_t encoding::SNEZ(RiscVRegister rd, RiscVRegister rs) {
	return SLTU(rd, RiscVRegister::zero, rs);
}

riscv_instruction_t encoding::SLTZ(RiscVRegister rd, RiscVRegister rs) {
	return SLT(rd, rs, RiscVRegister::zero);
}

riscv_instruction_t encoding::SGTZ(RiscVRegister rd, RiscVRegister rs) {
	return SLT(rd, RiscVRegister::zero, rs);
}

utils::riscv_instruction_t encoding::FMVS(RiscVFloatingRegister rd, RiscVFloatingRegister rs) {
	return FSGNJS(rd, rs, rs);
}

utils::riscv_instruction_t encoding::FABSS(RiscVFloatingRegister rd, RiscVFloatingRegister rs) {
	return FSGNJXS(rd, rs, rs);
}

utils::riscv_instruction_t encoding::FNEGS(RiscVFloatingRegister rd, RiscVFloatingRegister rs) {
	return FSGNJNS(rd, rs, rs);
}

utils::riscv_instruction_t encoding::FMVD(RiscVFloatingRegister rd, RiscVFloatingRegister rs) {
	return FSGNJD(rd, rs, rs);
}

utils::riscv_instruction_t encoding::FABSD(RiscVFloatingRegister rd, RiscVFloatingRegister rs) {
	return FSGNJXD(rd, rs, rs);
}

utils::riscv_instruction_t encoding::FNEGD(RiscVFloatingRegister rd, RiscVFloatingRegister rs) {
	return FSGNJND(rd, rs, rs);
}

riscv_instruction_t encoding::BEQZ(RiscVRegister rs, uint32_t offset) {
	return BEQ(rs, RiscVRegister::zero, offset);
}

riscv_instruction_t encoding::BNQZ(RiscVRegister rs, uint32_t offset) {
	return BNE(rs, RiscVRegister::zero, offset);
}

riscv_instruction_t encoding::BLEZ(RiscVRegister rs, uint32_t offset) {
	return BGE(RiscVRegister::zero, rs, offset);
}

riscv_instruction_t encoding::BGEZ(RiscVRegister rs, uint32_t offset) {
	return BGE(rs, RiscVRegister::zero, offset);
}

riscv_instruction_t encoding::BLTZ(RiscVRegister rs, uint32_t offset) {
	return BLT(rs, RiscVRegister::zero, offset);
}

riscv_instruction_t encoding::BGTZ(RiscVRegister rs, uint32_t offset) {
	return BEQ(RiscVRegister::zero, rs, offset);
}

riscv_instruction_t encoding::BGT(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return BLT(rs2, rs1, offset);
}

riscv_instruction_t encoding::BLE(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return BGE(rs2, rs1, offset);
}

riscv_instruction_t encoding::BGTU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return BLTU(rs2, rs1, offset);
}

riscv_instruction_t encoding::BLEU(RiscVRegister rs1, RiscVRegister rs2, uint32_t offset) {
	return BGEU(rs2, rs1, offset);
}

riscv_instruction_t encoding::J(uint32_t offset) {
	return JAL(RiscVRegister::zero, offset);
}
