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
		 * @param x86_instruction The x86 instruction object.
		 * @param riscv_instructions An array of risc-v instructions.
		 * @param num_instructions current number of risc-v instructions.
		 * @param index operand-index of instruction.
		 * @param reg The resulting address will be returned in this register. (t6 may not be used)
		 * @return The index of the first free instruction (i.e. current number of instructions).
		 */
		static size_t translate_memory_operand(const fadec::Instruction& x86_instruction,
											   utils::riscv_instruction_t* riscv_instructions, size_t num_instructions,
											   size_t index, encoding::RiscVRegister reg);

		size_t translate_mov(const fadec::Instruction& x86_instruction, utils::riscv_instruction_t* riscv_instruction);

		size_t translate_ret(const fadec::Instruction& x86_instruction, utils::riscv_instruction_t* riscv_instruction);

		static size_t load_12bit_immediate(uint16_t immediate, encoding::RiscVRegister destination,
										   utils::riscv_instruction_t* riscv_instructions, size_t num_instructions);

		static size_t
		load_32bit_immediate(uint32_t immediate, encoding::RiscVRegister destination,
							 utils::riscv_instruction_t* riscv_instructions, size_t num_instructions);

		static size_t
		load_64bit_immediate(uint64_t immediate, encoding::RiscVRegister destination,
							 utils::riscv_instruction_t* riscv_instructions, size_t num_instructions);

		/**
		 * Load an immediate of arbitrary size into the register. It will be automatically checked how long the immediate is.
		 * If the immediate is <=32 bit it will be sign extended to 64 bit.
		 * @param immediate The immediate that will be loaded.
		 * @param destination The regiser in which the immediate will be loaded.
		 * @param riscv_instructions The pointer to the generated riscv instructions.
		 * @param num_instructions The current length of the riscv instructions (i.e. the index of the next free position).
		 * @return The index of the next free riscv instruction (i.e. new length).
		 */
		static size_t
		load_immediate(uintptr_t immediate, encoding::RiscVRegister destination,
					   utils::riscv_instruction_t* riscv_instructions, size_t num_instructions);

		static size_t
		load_unsigned_immediate(uintptr_t immediate, encoding::RiscVRegister destination,
								utils::riscv_instruction_t* riscv_instructions, size_t num_instructions);
	};
}

#endif //OXTRA_CODEGEN_H