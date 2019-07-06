#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;

bool CodeGenerator::translate_instruction(const Instruction& inst, riscv_instruction_t* riscv, size_t& count) {
	switch (inst.get_type()) {
		// at the moment we just insert a return for every instruction that modifies control flow.
		case InstructionType::JMP:
		case InstructionType::CALL:
		case InstructionType::RET:
		case InstructionType::RET_IMM:
			count = translate_ret(inst, riscv);
			return true;

		case InstructionType::PUSH:
		case InstructionType::POP:
			break;

		case InstructionType::LEA:
			//[0xFFFFFFFF + 0x321*8 + 0x12345678] = 0x1_1234_6F7F
			load_unsigned_immediate(0xFFFFFFFF, RiscVRegister::a1, riscv, count);
			load_unsigned_immediate(0x321, RiscVRegister::a2, riscv, count);
			translate_memory_operand(inst, 1, RiscVRegister::a0, riscv, count);
			break;

		case InstructionType::NOP:
			break;

		case InstructionType::MOV_IMM:
		case InstructionType::MOVABS_IMM:
		case InstructionType::MOV:
		case InstructionType::MOVSX:
		case InstructionType::MOVZX:
			count += translate_mov(inst, riscv);
			break;

		default:
			throw std::runtime_error("Unsupported instruction used.");
	}

	return false;
}

size_t CodeGenerator::translate_mov(const Instruction& inst, riscv_instruction_t* riscv) {
	riscv[0] = LUI(RiscVRegister::a0, static_cast<uint16_t>(inst.get_immediate()));
	return 1;
}

size_t CodeGenerator::translate_ret(const Instruction& inst, riscv_instruction_t* riscv) {
	riscv[0] = JALR(RiscVRegister::zero, RiscVRegister::ra, 0);
	return 1;
}
