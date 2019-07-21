#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>
#include <oxtra/dispatcher/dispatcher.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;
using namespace dispatcher;

void CodeGenerator::translate_add(const ContextInstruction& inst, encoding::RiscVRegister dest,
								  encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);
	riscv[count++] = encoding::ADD(dest, src, dest);
}

void CodeGenerator::translate_inc(const ContextInstruction& inst, encoding::RiscVRegister dest,
								  encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);
	unused_parameter(src);

	riscv[count++] = encoding::ADDI(dest, dest, 1);
}

void CodeGenerator::translate_sub(const ContextInstruction& inst, encoding::RiscVRegister dest,
								  encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);

	riscv[count++] = encoding::SUB(dest, dest, src);
}

void CodeGenerator::translate_dec(const ContextInstruction& inst, encoding::RiscVRegister dest,
								  encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);
	unused_parameter(src);

	riscv[count++] = encoding::ADDI(dest, dest, -1);
}

void CodeGenerator::translate_neg(const ContextInstruction& inst, encoding::RiscVRegister dest,
								  encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);
	unused_parameter(src);

	riscv[count++] = encoding::NEG(dest, dest);
}

void CodeGenerator::translate_imul(const ContextInstruction& inst, encoding::RiscVRegister dest,
								   encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);

	riscv[count++] = encoding::MUL(dest, src, dest);
}

void CodeGenerator::translate_shl(const ContextInstruction& inst, encoding::RiscVRegister dest,
								  encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);

	riscv[count++] = encoding::SLL(dest, dest, src);
}

void CodeGenerator::translate_shr(const ContextInstruction& inst, encoding::RiscVRegister dest,
								  encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);

	riscv[count++] = encoding::SRL(dest, dest, src);
}

void CodeGenerator::translate_sar(const ContextInstruction& inst, encoding::RiscVRegister dest,
								  encoding::RiscVRegister src, utils::riscv_instruction_t* riscv, size_t& count) {
	unused_parameter(inst);

	riscv[count++] = encoding::SRA(dest, dest, src);
}

void CodeGenerator::translate_mov_ext(const ContextInstruction& inst, RiscVRegister dest, RiscVRegister src,
									  utils::riscv_instruction_t* riscv, size_t& count) {
	/* Thus they will have to be sign-extended/zero-extended.
	 * Otherwise the optimization will fail (load full 8-byte register, and store the interesting parts).
	 * [It will fail, because the interesting, stored parts, are larger than they should be]
	 * With a simple hack of shifting all the way up, and down again, we can fill the space with the
	 * highest bit. */
	const auto shift_amount = 64 - 8 * inst.get_operand(1).get_size();
	riscv[count++] = encoding::SLLI(dest, src, shift_amount);
	if (inst.get_type() == InstructionType::MOVSX)
		riscv[count++] = encoding::SRAI(dest, dest, shift_amount);
	else
		riscv[count++] = encoding::SRLI(dest, dest, shift_amount);
}

void CodeGenerator::translate_mov(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	const auto& dst_operand = inst.get_operand(0);
	const auto& src_operand = inst.get_operand(1);

	if (dst_operand.get_type() == OperandType::reg) {
		const auto dst_register = map_reg(dst_operand.get_register());

		// if the destination operand is a 8-byte register then we can move the source operand result directly into it
		if (dst_operand.get_size() == 8) {
			translate_operand(inst, 1, dst_register, RiscVRegister::t0, RiscVRegister::t1, riscv, count);
			return;
		}

			/*
			 * if the destination operand is a 4-byte register then we can move the source operand result into it
			 * and clear the upper 32 bits (saves 1 instruction)
			 */
		else if (dst_operand.get_size() == 4) {
			translate_operand(inst, 1, dst_register, RiscVRegister::t0, RiscVRegister::t1, riscv, count);
			riscv[count++] = encoding::SLLI(dst_register, dst_register, 32);
			riscv[count++] = encoding::SRLI(dst_register, dst_register, 32);
			return;
		}
	}

	// extract the source-operand
	RiscVRegister source_operand = RiscVRegister::t0;
	if (src_operand.get_type() == OperandType::reg && src_operand.get_register_type() != RegisterType::gph) {
		source_operand = map_reg(src_operand.get_register());
	} else
		translate_operand(inst, 1, source_operand, RiscVRegister::t1, RiscVRegister::t2, riscv, count);

	// write the value to the destination-register
	translate_destination(inst, source_operand, RiscVRegister::zero, RiscVRegister::t1, RiscVRegister::t2, riscv, count);
}

void CodeGenerator::translate_jmp(const ContextInstruction& inst, riscv_instruction_t* riscv, size_t& count) {
	if (inst.get_operand(0).get_type() == OperandType::imm) {
		load_64bit_immediate(inst.get_immediate(), address_destination, riscv, count, false);
		riscv[count++] = JALR(RiscVRegister::ra, reroute_static_address, 0);
	} else {
		translate_operand(inst, 0, address_destination, RiscVRegister::t0, RiscVRegister::t1, riscv, count);
		riscv[count++] = JALR(RiscVRegister::ra, reroute_dynamic_address, 0);
	}
}

void CodeGenerator::translate_syscall(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	// add the syscall-jump and add the padding for a static reroute
	riscv[count++] = JALR(RiscVRegister::ra, syscall_address, 0);
	load_64bit_immediate(inst.get_address() + inst.get_size(), address_destination, riscv, count, false);
	riscv[count++] = JALR(RiscVRegister::ra, reroute_static_address, 0);
}

void CodeGenerator::translate_push(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	const auto& operand = inst.get_operand(0);
	constexpr auto rsp_reg = map_reg(Register::rsp);

	// update the stack-pointer
	riscv[count++] = encoding::ADDI(rsp_reg, rsp_reg, -operand.get_size());

	// extract the value
	RiscVRegister value_register = RiscVRegister::t0;
	if (operand.get_type() == OperandType::reg)
		value_register = map_reg(operand.get_register());
	else
		translate_operand(inst, 0, value_register, RiscVRegister::t1, RiscVRegister::t2, riscv, count);

	// write the value to memory
	switch (operand.get_size()) {
		case 8:
			riscv[count++] = encoding::SD(rsp_reg, value_register, 0);
			break;
		case 4:
			riscv[count++] = encoding::SW(rsp_reg, value_register, 0);
			break;
		case 2:
			riscv[count++] = encoding::SH(rsp_reg, value_register, 0);
			break;
	}
}

void CodeGenerator::translate_pushf(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	// update the stack-pointer
	constexpr auto rsp_reg = map_reg(Register::rsp);
	riscv[count++] = encoding::ADDI(rsp_reg, rsp_reg, -inst.get_operand_size());

	// write the value to memory
	switch (inst.get_operand_size()) {
		case 8:
			riscv[count++] = encoding::SD(rsp_reg, flag_register, 0);
			break;
		case 4:
			riscv[count++] = encoding::SW(rsp_reg, flag_register, 0);
			break;
		case 2:
			riscv[count++] = encoding::SH(rsp_reg, flag_register, 0);
			break;
	}
}

void CodeGenerator::translate_pop(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	constexpr auto rsp_reg = map_reg(Register::rsp);
	const auto operand_size = inst.get_operand(0).get_size();

	// if the destination operand is a register then we can optimize it
	if (inst.get_operand(0).get_type() == OperandType::reg) {
		const auto dest_reg = map_reg(inst.get_operand(0).get_register());
		switch (operand_size) {
			case 8:
				riscv[count++] = encoding::LD(dest_reg, rsp_reg, 0);
				break;
			case 4:
				riscv[count++] = encoding::LWU(RiscVRegister::t0, rsp_reg, 0);
				move_to_register(dest_reg, RiscVRegister::t0, RegisterAccess::DWORD, RiscVRegister::t1, riscv, count, true);
				break;
			case 2:
				riscv[count++] = encoding::LHU(RiscVRegister::t0, rsp_reg, 0);
				move_to_register(dest_reg, RiscVRegister::t0, RegisterAccess::WORD, RiscVRegister::t1, riscv, count, true);
				break;
		}
	} else if (inst.get_operand(0).get_type() == OperandType::mem) {
		RiscVRegister reg = translate_memory(inst, 0, RiscVRegister::t0, RiscVRegister::t1, riscv, count);
		switch (operand_size) {
			case 8:
				riscv[count++] = encoding::LD(RiscVRegister::t1, rsp_reg, 0);
				riscv[count++] = encoding::SD(reg, RiscVRegister::t1, 0);
				break;
			case 4:
				riscv[count++] = encoding::LW(RiscVRegister::t1, rsp_reg, 0);
				riscv[count++] = encoding::SW(reg, RiscVRegister::t1, 0);
				break;
			case 2:
				riscv[count++] = encoding::LH(RiscVRegister::t1, rsp_reg, 0);
				riscv[count++] = encoding::SH(reg, RiscVRegister::t1, 0);
				break;
		}
	}

	// update the stack pointer
	riscv[count++] = encoding::ADDI(rsp_reg, rsp_reg, operand_size);
}

void CodeGenerator::translate_popf(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	constexpr auto rsp_reg = map_reg(Register::rsp);

	// load the top of the stack into the flags register
	switch (inst.get_operand_size()) {
		case 8:
			riscv[count++] = encoding::LD(flag_register, rsp_reg, 0);
			break;
		case 4:
			riscv[count++] = encoding::LW(flag_register, rsp_reg, 0);
			break;
		case 2:
			riscv[count++] = encoding::LH(flag_register, rsp_reg, 0);
			break;
	}

	// update the stack pointer
	riscv[count++] = encoding::ADDI(rsp_reg, rsp_reg, inst.get_operand_size());
}

void CodeGenerator::translate_ret(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	constexpr auto rsp_reg = map_reg(Register::rsp);

	// pop the ip from the stack
	riscv[count++] = encoding::LD(address_destination, rsp_reg, 0);

	// pop the immediate from the stack
	if (inst.get_immediate() + 8 < 0x800)
		riscv[count++] = encoding::ADDI(rsp_reg, rsp_reg, inst.get_immediate() + 8);
	else {
		load_unsigned_immediate(inst.get_immediate() + 8, RiscVRegister::t0, riscv, count);
		riscv[count++] = encoding::ADD(rsp_reg, rsp_reg, RiscVRegister::t0);
	}
	
	// attach the rerouting
	riscv[count++] = JALR(RiscVRegister::ra, reroute_dynamic_address, 0);
}

void CodeGenerator::translate_call(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count) {
	// add the instructions for pushing the ip
	load_unsigned_immediate(inst.get_address() + inst.get_size(), RiscVRegister::t0, riscv, count);
	riscv[count++] = encoding::ADDI(map_reg(Register::rsp), map_reg(Register::rsp), -8);
	riscv[count++] = encoding::SD(map_reg(Register::rsp), RiscVRegister::t0, 0);

	// the rest of the function behaves just like jump
	translate_jmp(inst, riscv, count);
}