#ifndef OXTRA_CODEGEN_H
#define OXTRA_CODEGEN_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codestore/codestore.h"
#include "oxtra/elf/elf.h"
#include "oxtra/codegen/encoding/encoding.h"
#include "oxtra/codegen/decoding/decoding.h"
#include <fadec.h>

namespace codegen {
	class CodeGenerator {
	private:
		// If these registers are changed, the documentation has to be updated
		constexpr static encoding::RiscVRegister
				memory_temp_register = encoding::RiscVRegister::t6,
				read_temp_register = encoding::RiscVRegister::t6;
		constexpr static encoding::RiscVRegister
				temp0_register = encoding::RiscVRegister::t0,
				temp1_register = encoding::RiscVRegister::t1,
				temp2_register = encoding::RiscVRegister::t2;

		static constexpr encoding::RiscVRegister register_mapping[] = {
				encoding::RiscVRegister::a0, // rax
				encoding::RiscVRegister::a2, // rcx
				encoding::RiscVRegister::a3, // rdx
				encoding::RiscVRegister::a1, // rbx
				encoding::RiscVRegister::sp, // rsp
				encoding::RiscVRegister::s0, // rbp
				encoding::RiscVRegister::a4, // rsi
				encoding::RiscVRegister::a5, // rdi
				encoding::RiscVRegister::a6, // r8
				encoding::RiscVRegister::a7, // r9
				encoding::RiscVRegister::s2, // r10
				encoding::RiscVRegister::s3, // r11
				encoding::RiscVRegister::s4, // r12
				encoding::RiscVRegister::s5, // r13
				encoding::RiscVRegister::s6, // r14
				encoding::RiscVRegister::s7  // r15

				/*
				 * ra, gp, tp : reserved
				 * t0, t1, t2 : always temporary
				 * t4, t5, t6 : reserved for helper functions
				 * t3 : reserved
				 * s1 : flags
				 * s8 : host_call address
				 * s9 : inline_translate address
				 * s10 : reserved for system calls
				 * s11 : dispatcher address
				 */
		};

		enum class RegisterAccess : uint8_t {
			QWORD,    //64bit
			DWORD,    //32bit
			WORD,    //16bit
			HBYTE,
			LBYTE
		};

		// define the instruction-flag-groups (basic-flags: ZERO;SIGN;CARRY;OVERFLOW)
		struct Group {
			constexpr static size_t none = 0x0000u;
			constexpr static size_t require_zero = 0x0001u;
			constexpr static size_t require_sign = 0x0002u;
			constexpr static size_t require_carry = 0x0004u;
			constexpr static size_t require_overflow = 0x0008u;
			constexpr static size_t require_basic = require_zero | require_sign | require_carry | require_overflow;
			constexpr static size_t require_parity = 0x0010u;
			constexpr static size_t require_auxiliary = 0x0020u;
			constexpr static size_t require_direction = 0x0040u;
			constexpr static size_t require_all = require_basic | require_parity | require_auxiliary | require_direction;

			constexpr static size_t update_zero = 0x0100u;
			constexpr static size_t update_sign = 0x0200u;
			constexpr static size_t update_carry = 0x0400u;
			constexpr static size_t update_overflow = 0x0800u;
			constexpr static size_t update_basic = update_zero | update_sign | update_carry | update_overflow;
			constexpr static size_t update_parity = 0x1000u;
			constexpr static size_t update_auxiliary = 0x2000u;
			constexpr static size_t update_direction = 0x4000u;
			constexpr static size_t update_all = update_basic | update_parity | update_auxiliary | update_direction;

			constexpr static size_t end_of_block = 0x8000u | require_all;
			constexpr static size_t error = 0xffffffffu;
			constexpr static size_t require_to_update_lshift = 16u;
		};
		struct InstructionEntry {
			fadec::Instruction instruction;
			size_t update_flags;
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
		/**
		 * Translates a basic block to riscv-instructions, and returns the starting address of that riscv-block.
		 * @param addr beginning of the basic block as x86-address.
		 * @return translated riscv-starting address.
		 */
		utils::host_addr_t translate(utils::guest_addr_t addr);

	private:
		/**
		 * Translates a x86 instruction into multiple risc-v instructions. Or Queries information about the given instruction
		 * @param inst instruction-object, which either has to be parsed, or filled.
		 * @param riscv Array of riscv instructions. (If null, only query information)
		 * @param count Number of riscv instructions. (If null, only query information)
		 * @return The Group-information about the instruction.
		 */
		size_t translate_instruction(InstructionEntry& inst, utils::riscv_instruction_t* riscv, size_t* count);

		/**
		 * Translates MOV-instructions (MOV;MOVABS;MOVIMM;MOVSX;MOVZX)
		 * @param inst The x86 instruction object.
		 * @param riscv An array of riscv instructions.
		 * @param count Reference to the number of instructions that were written to the array.
		 */
		void translate_mov(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Translates a RET-instruction
		 * @param inst The x86 instruction object.
		 * @param riscv An array of riscv instructions.
		 * @param count Reference to the number of instructions that were written to the array.
		 */
		void translate_ret(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Translates a x86-memory operand into risc-v instructions (resulting address in reg)
		 * t6 might be overridden.
		 * @param x86_instruction The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param reg The resulting address will be returned in this register. (t6 may not be used)
		 * @param riscv_instructions An array of risc-v instructions.
		 * @param num_instructions current number of risc-v instructions.
		 * @return The index of the first free instruction (i.e. current number of instructions).
		 */
		void memory_operand(const fadec::Instruction& inst, size_t index, encoding::RiscVRegister reg,
							utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Writes a register with x86-style sub-manipulation to an existing register without
		 * invalidating the rest of the value.
		 *
		 * for example:
		 * 		- read x86:ah from riscv:a1
		 * 		- manipulate riscv:a1
		 * 		- store riscv:a1 to x86:eax
		 *
		 * The source-register will be preserved.
		 * t5 and t6 might be overwritten.
		 * @param dest register to be changed.
		 * @param src register to write.
		 * @param access the operand-size of the register to write to.
		 * @param riscv An array of risc-v instructions.
		 * @param count current number of risc-v instructions.
		 */
		static void move_to_register(encoding::RiscVRegister dest, encoding::RiscVRegister src, RegisterAccess access,
									 utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Reads a register with x86-style sub-manipulation from an existing register.
		 *
		 * for example:
		 * 		- read x86:ah from riscv:a1
		 * 		- manipulate riscv:a1
		 * 		- store riscv:a1 to x86:eax
		 *
		 * The source-register will be preserved.
		 * t5 and t6 might be overwritten.
		 * @param dest register to be changed.
		 * @param src register to read.
		 * @param access the operand-size of the register to read from.
		 * @param riscv An array of risc-v instructions.
		 * @param count current number of risc-v instructions.
		 */
		static void get_from_register(encoding::RiscVRegister dest, encoding::RiscVRegister src, RegisterAccess access,
									  utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Loads a 12 bit immediate into the specified register. The value is sign extended to 64 bit,
		 * so be careful when using this method to ensure that the value is <= 11 bits or negative numbers are desired.
		 * Use load_unsigned_immediate otherwise.
		 * @param imm The immediate that will be stored. The highest 4 bits of uint16_t will be masked.
		 * @param dest The register the immediate will be stored in.
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count A reference to the length of the instructions (has to point to the first free index)
		 */
		static void load_12bit_immediate(uint16_t imm, encoding::RiscVRegister dest, utils::riscv_instruction_t* riscv,
										 size_t& count);

		/**
		 * Loads a 32 bit immediate into the specified register.
		 * The value is sign extended to 64 bit, so be careful when using this method
		 * to ensure that the value is <= 31 bits or negative numbers are desired.
		 * Use load_unsigned_immediate otherwise.
		 * @param imm The immediate that will be stored. The highest 4 bits of uint16_t will be masked.
		 * @param dest The register the immediate will be stored in.
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count A reference to the length of the instructions (has to point to the first free index)
		 */
		static void load_32bit_immediate(uint32_t imm, encoding::RiscVRegister dest, utils::riscv_instruction_t* riscv,
										 size_t& count);

		/**
		 * Loads a 64 bit immediate into the specified register.
		 * The only difference between this method and load_unsigned_immediate is that this one is slightly faster.
		 * @param imm The immediate that will be stored. The highest 4 bits of uint16_t will be masked.
		 * @param dest The register the immediate will be stored in.
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count A reference to the length of the instructions (has to point to the first free index)
		 */
		static void load_64bit_immediate(uint64_t imm, encoding::RiscVRegister dest, utils::riscv_instruction_t* riscv,
										 size_t& count);

		/**
		 * Load an immediate up to 64 bit into the specified register.
		 * It will be automatically checked how long the immediate is. If the immediate is <=32 bit it will always
		 * be sign extended to 64 bit.
		 * @param imm The immediate that will be loaded.
		 * @param dest The regiser in which the immediate will be loaded.
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
		 */
		static void
		load_signed_immediate(uintptr_t imm, encoding::RiscVRegister dest, utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Load an immediate of up to 64 bit into the register.
		 * The immediate will not be sign extended (i.e. treated as unsigned) unless it is 64 bit (where sign extension
		 * never happens).
		 * @param imm The immediate that will be loaded.
		 * @param dest The regiser in which the immediate will be loaded.
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
		 */
		static void
		load_unsigned_immediate(uintptr_t imm, encoding::RiscVRegister dest, utils::riscv_instruction_t* riscv, size_t& count);
	};
}

#endif //OXTRA_CODEGEN_H
