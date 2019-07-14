#ifndef OXTRA_CODEGEN_H
#define OXTRA_CODEGEN_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codestore/codestore.h"
#include "oxtra/elf/elf.h"
#include "oxtra/codegen/encoding/encoding.h"
#include "oxtra/codegen/decoding/decoding.h"
#include <fadec.h>
#include "oxtra/dispatcher/context.h"

namespace codegen {
	namespace Group {
		using Type = uint32_t;
		enum : Type {
			none = 0x0000u,
			require_zero = 0x0001u,
			require_sign = 0x0002u,
			require_carry = 0x0004u,
			require_overflow = 0x0008u,
			require_parity = 0x0010u,
			require_all = require_zero | require_sign | require_carry | require_overflow | require_parity,
			update_zero = 0x0100u,
			update_sign = 0x0200u,
			update_carry = 0x0400u,
			update_overflow = 0x0800u,
			update_parity = 0x1000u,
			update_all = update_zero | update_sign | update_carry | update_overflow | update_parity,
			end_of_block = 0x8000u | require_all,
			error = 0xffffffffu,
			require_to_update_lshift = 8u
		};
	}

	class CodeGenerator {
	public:
		// If these registers are changed, the documentation has to be updated
		constexpr static encoding::RiscVRegister
		/* functions, which use the temporary register t6 are
		 * obligated to only use it within the function
		 * and not call any other functions */
				memory_temp_register = encoding::RiscVRegister::t6,
				move_to_temp_register = encoding::RiscVRegister::t6,
				flags_temp_register = encoding::RiscVRegister::t6,

				dest_temp_register = encoding::RiscVRegister::t5,
				source_temp_register = encoding::RiscVRegister::t4,
				address_temp_register = encoding::RiscVRegister::t3,
				address_destination = encoding::RiscVRegister::t3,
				reroute_static_address = encoding::RiscVRegister::s8,
				reroute_dynamic_address = encoding::RiscVRegister::s9,
				syscall_address = encoding::RiscVRegister::s10,
				context_address = encoding::RiscVRegister::s11,
				temp0_register = encoding::RiscVRegister::t0,
				temp1_register = encoding::RiscVRegister::t1,
				temp2_register = encoding::RiscVRegister::t2,
				flag_register = encoding::RiscVRegister::s1;

		/*
		 * ra, gp, tp : reserved
		 * t0, t1, t2 : always temporary
		 * t3, t4, t5, t6 : reserved for helper functions
		 * t3 : address_destination [doubled with reserved for helper-functions]
		 * s1 : flags
		 * s8 : reroute_static address
		 * s9 : reroute_dynamic address
		 * s10 : reserved for system calls
		 * s11 : context address
		 */

	private:
		enum class RegisterAccess : uint8_t {
			QWORD,    // 64bit
			DWORD,    // 32bit
			WORD,    // 16bit
			HBYTE,    // high byte
			LBYTE    // low byte
		};

		/**
		 * An instruction that contains information based on the context.
		 */
		struct ContextInstruction : public fadec::Instruction {
			Group::Type update_flags = 0;
		};

		using OperationCallback = void (*)(const ContextInstruction& inst, encoding::RiscVRegister dest,
										   encoding::RiscVRegister source, utils::riscv_instruction_t* riscv,
										   size_t& count);

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

		void update_basic_block(utils::host_addr_t addr, utils::host_addr_t absolute_address);

	private:
		static void translate_mov_ext(const ContextInstruction& inst, encoding::RiscVRegister dest, encoding::RiscVRegister src,
									  utils::riscv_instruction_t* riscv, size_t& count);

		static void translate_mov(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		static void translate_jmp(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		// TODO: implement this properly
		static void translate_ret(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		static void translate_syscall(utils::riscv_instruction_t* riscv, size_t& count);

		static void translate_push(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		static void translate_pushf(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		static void translate_pop(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		static void translate_popf(const fadec::Instruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Translates a x86 instruction into multiple risc-v instructions.
		 * @param inst The x86 instruction object.
		 * @param riscv An array of riscv instructions.
		 * @param count Reference to the number of instructions that were written to the array.
		 * @return Returns whether the this instruction ends the basic block.
		 */
		static bool translate_instruction(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Extracts all of the grouping information out of the instruction.
		 * @param type The type of instructions
		 * @return the group-flags of the instruction
		 */
		static size_t group_instruction(const fadec::InstructionType type);

		/**
		 * Translates a single x86 instruction into an array of riscv-instructions.
		 * @param inst Instruction-object containig flag-information.
		 * @param riscv Array of riscv-instructions.
		 * @param count Number of riscv-instructions.
		 */
		static void translate_instruction(ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * extracts the two operands out of the instruction, and calls the callback,
		 * which then implements the corresponding operation.
		 * The value of the source-register must not be changed. For optimization-purposes,
		 * it might be the actual source-register.
		 * If the operand-size is not 64 bits, the registers might still have some undefined high bits.
		 * @param inst x86 decoded instruction.
		 * @param riscv Array of riscv-instructions.
		 * @param count Number of riscv-instructions.
		 * @param callback Callback, which will apply the instruction.
		 */
		static void apply_operation(const ContextInstruction& inst, utils::riscv_instruction_t* riscv, size_t& count,
									OperationCallback callback);

		/**
		 * Translates a single operand (either register, or memory or immediate value)
		 * @param inst The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param reg The resulting value will be stored in this register.
		 * @param riscv An array of risc-v instructions.
		 * @param count current number of risc-v instructions.
		 * @return if this operation was a memory-operation, the return-register will contain the address.
		 */
		static encoding::RiscVRegister translate_operand(const fadec::Instruction& inst, size_t index,
														 encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv,
														 size_t& count);

		/**
		 * Writes the value in the register to the destination-operand of the instruction
		 * The register will be preserved.
		 * @param inst The x86 instruction object.
		 * @param reg This value will be written to the destination.
		 * @param address If the destination is a memory address, this address will be used as destination.
		 * @param riscv An array of risc-v instructions.
		 * @param count current number of risc-v instructions.
		 */
		static void translate_destination(const fadec::Instruction& inst, encoding::RiscVRegister reg,
										  encoding::RiscVRegister address, utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Translates a x86-memory operand into risc-v instructions (resulting address in reg)
		 * t6 might be overridden.
		 * @param x86_instruction The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param reg The resulting address will be returned in this register. (t6 may not be used)
		 * @param riscv_instructions An array of risc-v instructions.
		 * @param num_instructions current number of risc-v instructions.
		 */
		static void translate_memory(const fadec::Instruction& inst, size_t index, encoding::RiscVRegister reg,
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
		 * @param dest Register to be changed.
		 * @param src Register to write.
		 * @param access The operand-size of the register to write to.
		 * @param riscv An array of risc-v instructions.
		 * @param count Current number of risc-v instructions.
		 * @param cleared If true the upper bits of the source register are expected to be 0.
		 */
		static void move_to_register(encoding::RiscVRegister dest, encoding::RiscVRegister src, RegisterAccess access,
									 utils::riscv_instruction_t* riscv, size_t& count, bool cleared = false);

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
		 * @param optimize When set to true, this method tries to minimize the number of generated instructions.
		 */
		static void load_32bit_immediate(uint32_t imm, encoding::RiscVRegister dest, utils::riscv_instruction_t* riscv,
										 size_t& count, bool optimize);

		/**
		 * Loads a 64 bit immediate into the specified register.
		 * The only difference between this method and load_unsigned_immediate is that this one is slightly faster.
		 * Unoptimized this function must generate 8-riscv-instructions. Otherwise update_basic_block will fail.
		 * @param imm The immediate that will be stored. The highest 4 bits of uint16_t will be masked.
		 * @param dest The register the immediate will be stored in.
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count A reference to the length of the instructions (has to point to the first free index)
		 * @param optimize When set to true, this method tries to minimize the number of generated instructions.
		 */
		static void load_64bit_immediate(uint64_t imm, encoding::RiscVRegister dest, utils::riscv_instruction_t* riscv,
										 size_t& count, bool optimize);

		/**
		 * Load an immediate up to 64 bit into the specified register.
		 * It will be automatically checked how long the immediate is. If the immediate is <=32 bit it will always
		 * be sign extended to 64 bit.
		 * This function always optimizes the number of instructions generated.
		 * @param imm The immediate that will be loaded.
		 * @param dest The regiser in which the immediate will be loaded.
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
		 */
		static void load_signed_immediate(uintptr_t imm, encoding::RiscVRegister dest,
										  utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Load an immediate of up to 64 bit into the register.
		 * The immediate will not be sign extended (i.e. treated as unsigned) unless it is 64 bit (where sign extension
		 * never happens).
		 * This function always optimizes the number of instructions generated.
		 * @param imm The immediate that will be loaded.
		 * @param dest The regiser in which the immediate will be loaded.
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
		 */
		static void
		load_unsigned_immediate(uintptr_t imm, encoding::RiscVRegister dest,
								utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Update the zero-flag from the result-register. The register will be unchanged.
		 * @param reg register which contains the result-value
		 * @param reg_size operand-size of the register (8,4,2,1)
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
		 */
		static void
		update_zero_flag(encoding::RiscVRegister reg, uint8_t reg_size, utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Update the sign-flag from the result-register. The register will be unchanged.
		 * @param reg register which contains the result-value
		 * @param reg_size operand-size of the register (8,4,2,1)
		 * @param riscv The pointer to the generated riscv instructions.
		 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
		 */
		static void
		update_sign_flag(encoding::RiscVRegister reg, uint8_t reg_size, utils::riscv_instruction_t* riscv, size_t& count);

		static void update_carry_flag(encoding::RiscVRegister src1, encoding::RiscVRegister src2, uint8_t reg_size,
									  utils::riscv_instruction_t* riscv, size_t& count);

		/**
		 * Returns the riscv-register, which maps to the x86-register.
		 * @param reg The x86-register.
		 * @return The riscv-register.
		 */
		static constexpr encoding::RiscVRegister map_reg(const fadec::Register reg) {
			constexpr encoding::RiscVRegister register_mapping[] = {
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
			};

			return register_mapping[static_cast<uint8_t>(reg)];
		}

		/**
		 * Maps the x86 gph register to it's base's riscv register.
		 * @param reg The x86-register.
		 * @return The riscv-register.
		 */
		static constexpr encoding::RiscVRegister map_reg_high(const fadec::Register reg) {
			return map_reg(static_cast<fadec::Register>(static_cast<uint8_t>(reg) + 4));
		}
	};
}

#endif //OXTRA_CODEGEN_H
