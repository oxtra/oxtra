#ifndef OXTRA_CODEGEN_H
#define OXTRA_CODEGEN_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codestore/codestore.h"
#include "oxtra/elf/elf.h"
#include "oxtra/codegen/encoding/encoding.h"
#include <fadec.h>

namespace codegen {
	class CodeGenerator {
	private:
		static constexpr encoding::RiscVRegister register_mapping[] = {
				encoding::RiscVRegister::a0, //rax
				encoding::RiscVRegister::a2, //rcx
				encoding::RiscVRegister::a3, //rdx
				encoding::RiscVRegister::a1, //rbx
				encoding::RiscVRegister::sp, //rsp
				encoding::RiscVRegister::s0, //rbp
				encoding::RiscVRegister::a4, //rsi
				encoding::RiscVRegister::a5, //rdi
				encoding::RiscVRegister::a6, //r8
				encoding::RiscVRegister::a7, //r9
				encoding::RiscVRegister::s2, //r10
				encoding::RiscVRegister::s3, //r11
				encoding::RiscVRegister::s4, //r12
				encoding::RiscVRegister::s5, //r13
				encoding::RiscVRegister::s6, //r14
				encoding::RiscVRegister::s7  //r15
		};

	private:
		const arguments::Arguments& _args;
		const elf::Elf& _elf;
		codestore::CodeStore _codestore;

	public:
		CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf);

		CodeGenerator(CodeGenerator&) = delete;

		CodeGenerator(CodeGenerator&&) = delete;

	public:
		utils::host_addr_t translate(utils::guest_addr_t addr);

	private:
		/**
		 * Translates a x86 instruction into multiple risc-v instructions.
		 * @param x86_instruction The x86 instruction object.
		 * @param riscv_instructions An array of riscv instructions.
		 * @param num_instructions Reference to the number of instructions that were written to the array.
		 * @return Returns whether the this instruction ends the basic block.
		 */
		bool
		translate_instruction(const fadec::Instruction& x86_instruction, utils::riscv_instruction_t* riscv_instructions,
							  size_t& num_instructions);

		/**
		 * Translates a x86-memory operand into risc-v instructions (resulting address in reg)
		 * t6 might be overridden.
		 * @param inst The x86 instruction object.
		 * @param riscv An array of risc-v instructions.
		 * @param count current number of risc-v instructions.
		 * @param index operand-index of instruction.
		 * @param reg The resulting address will be returned in this register.
		 */
		static void translate_memory_operand(const fadec::Instruction& inst, size_t index, encoding::RiscVRegister reg,
											 utils::riscv_instruction_t* riscv, size_t& count);

		static void load_immediate(uintptr_t immediate, encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv,
								   size_t& count);

		/**
		 * Writes a register of different size to an existing register without invalidating the rest of the value.
		 * The source-register will be preserved.
		 * t4 and t5 might be overriden.
		 * @param dest register to be changed
		 * @param src register to write
		 * @param size operand-size (in bytes) [0 means 1-byte high register]
		 * @param riscv An array of risc-v instructions
		 * @param count current number of risc-v instructions.
		 */
		static void move_to_register(encoding::RiscVRegister dest, encoding::RiscVRegister src, uint8_t size,
									 utils::riscv_instruction_t* riscv, size_t& count);

		size_t translate_mov(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv);

		size_t translate_ret(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv);
	};
}

#endif //OXTRA_CODEGEN_H