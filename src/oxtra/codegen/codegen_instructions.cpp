#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>
#include <oxtra/dispatcher/dispatcher.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;
using namespace dispatcher;

extern "C" int guest_exit();

void CodeGenerator::translate_mov_ext(const fadec::Instruction& inst, encoding::RiscVRegister dest, encoding::RiscVRegister src,
									  utils::riscv_instruction_t* riscv, size_t& count) {
	/* if this in struction is a movsx/movzx instruction,
	 * the input operands will vary in size. Thus they will have to be sign-extended/zero-extended.
	 * Otherwise the optimization will fail (load full 8-byte register, and store the interesting parts).
	 * [It will fail, because the interesting, stored parts, are larger than they should be]
	 * With a simple hack of shifting all the way up, and down again, we can fill the space with the
	 * highest bit. */
	riscv[count++] = encoding::SLLI(src, src, 64 - 8 * inst.get_operand(1).get_size());
	if (inst.get_type() == InstructionType::MOVSX)
		riscv[count++] = encoding::SRAI(dest, src, 64 - 8 * inst.get_operand(1).get_size());
	else
		riscv[count++] = encoding::SRLI(dest, src, 64 - 8 * inst.get_operand(1).get_size());
}

void CodeGenerator::translate_mov(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	// extract the source-operand
	RiscVRegister source_operand = temp0_register;
	if (inst.get_operand(1).get_type() == OperandType::reg &&
		inst.get_operand(1).get_register_type() != RegisterType::gph) {
		source_operand = register_mapping[static_cast<uint16_t>(inst.get_operand(1).get_register())];
	} else
		translate_operand(inst, 1, source_operand, riscv, count);

	// write the value to the destination-register
	translate_destination(inst, source_operand, RiscVRegister::zero, riscv, count);
}

void CodeGenerator::translate_jmp(const Instruction& inst, riscv_instruction_t* riscv, size_t& count) {
	if (inst.get_operand(0).get_type() == OperandType::imm) {
		load_64bit_immediate(inst.get_immediate(), address_destination, riscv, count, false);
		riscv[count++] = JALR(RiscVRegister::ra, reroute_static_address, 0);
	} else {
		translate_operand(inst, 0, address_destination, riscv, count);
		riscv[count++] = JALR(RiscVRegister::ra, reroute_dynamic_address, 0);
	}
	riscv[count++] = JALR(RiscVRegister::zero, address_destination, 0);
}

void CodeGenerator::translate_ret(const Instruction& inst, riscv_instruction_t* riscv, size_t& count) {
	load_64bit_immediate(reinterpret_cast<uint64_t>(guest_exit), temp0_register, riscv, count, false);
	riscv[count++] = JALR(RiscVRegister::zero, temp0_register, 0);
}
