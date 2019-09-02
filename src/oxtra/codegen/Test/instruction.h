#ifndef OXTRA_INSTRUCTION_H
#define OXTRA_INSTRUCTION_H

#include "oxtra/utils/types.h"
#include "jumptable/jump_table.h"
#include "oxtra/dispatcher/dispatcher.h"

namespace codegen {
	class Instruction : protected fadec::Instruction {
	public:
		enum Flags : uint8_t {
			none = 0x00,
			carry = 0x01,
			zero = 0x02,
			sign = 0x04,
			overflow = 0x08,
			parity = 0x10,
			all = carry | zero | sign | overflow | parity
		};

		struct FlagInfo {
			uint64_t zero_value;
			uint64_t parity_value;
			uint64_t sign_value;
			uint64_t overflow_value[2];
			uint64_t carry_value[2];
			uint16_t overflow_operation;
			uint16_t carry_operation;
			uint8_t sign_size;

			static constexpr uint32_t
					flag_info_offset = 0x1F8,
					zero_value_offset = flag_info_offset + sizeof(uint64_t) * 0,
					parity_value_offset = flag_info_offset + sizeof(uint64_t) * 1,
					sign_value_offset = flag_info_offset + sizeof(uint64_t) * 2,
					overflow_values_offset = flag_info_offset + sizeof(uint64_t) * 3,
					carry_values_offset = flag_info_offset + sizeof(uint64_t) * 5,
					overflow_operation_offset = flag_info_offset + sizeof(uint64_t) * 7,
					carry_operation_offset = flag_info_offset + sizeof(uint64_t) * 7 + sizeof(uint16_t),
					sign_size_offset = flag_info_offset + sizeof(uint64_t) * 7 + sizeof(uint16_t) * 2;
		};

		// If these registers are changed, the documentation has to be updated
		constexpr static encoding::RiscVRegister
				address_destination = encoding::RiscVRegister::t3,
				jump_table_address = encoding::RiscVRegister::s10,
				context_address = encoding::RiscVRegister::s11;

		/*
		 * sp : Is required to be the stack-pointer. Otherwise assembly will fail
		 * ra, gp, tp : reserved
		 * t0, t1, t2 : always temporary
		 * t3, t4, t5, t6 : reserved for helper functions
		 * t3 : address_destination [doubled with reserved for helper-functions]
		 * s10 : jump table
		 * s11 : context address
		 * s1, s8, s9: unmapped
		 */

	protected:
		enum class RegisterAccess : uint8_t {
			QWORD,
			DWORD,
			WORD,
			HBYTE,
			LBYTE
		};
	private:
		uint8_t update_flags;
		uint8_t require_flags;

	protected:
		explicit Instruction(uint8_t update, uint8_t require);

	public:
		virtual void generate(CodeBatch& batch) = 0;

		uint8_t query_require();

		uint8_t query_update();

		void set_require(uint8_t flags);

	protected:
		/**
		 * Translates a single operand (either register, or memory or immediate value)
		 * @param batch Store the current riscv-batch.
		 * @param inst The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param reg The resulting value will be stored in this register.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @return if this operation was a memory-operation,
		 * 		   the return-register will contain the address (either temp_a, or a base-register)
		 */
		encoding::RiscVRegister translate_operand(CodeBatch& batch, size_t index,
														 encoding::RiscVRegister reg, encoding::RiscVRegister temp_a,
														 encoding::RiscVRegister temp_b);

		/**
		 * Writes the value in the register to the destination-operand of the instruction
		 * The register will be preserved.
		 * @param batch Store the current riscv-batch.
		 * @param inst The x86 instruction object.
		 * @param reg This value will be written to the destination.
		 * @param address If the destination is a memory address, this address will be used as destination.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 */
		void translate_destination(CodeBatch& batch, encoding::RiscVRegister reg,
										  encoding::RiscVRegister address, encoding::RiscVRegister temp_a,
										  encoding::RiscVRegister temp_b);

		/**
		 * Translates a x86-memory operand into risc-v instructions .
		 * @param batch Store the current riscv-batch.
		 * @param x86_instruction The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @return Returns the register containing the address (either temp_a, or a base-register)
		 */
		encoding::RiscVRegister translate_memory(CodeBatch& batch, size_t index,
														encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b);

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
		 * @param batch Store the current riscv-batch.
		 * @param dest Register to be changed.
		 * @param src Register to write.
		 * @param access The operand-size of the register to write to.
		 * @param temp A temporary that might be changed.
		 * @param cleared If true the upper bits of the source register are expected to be 0.
		 */
		static void move_to_register(CodeBatch& batch, encoding::RiscVRegister dest, encoding::RiscVRegister src,
									 RegisterAccess access, encoding::RiscVRegister temp, bool cleared = false);

		/**
		 * Loads a 12 bit immediate into the specified register. The value is sign extended to 64 bit,
		 * so be careful when using this method to ensure that the value is <= 11 bits or negative numbers are desired.
		 * Use load_unsigned_immediate otherwise.
		 * @param batch Store the current riscv-batch.
		 * @param imm The immediate that will be stored. The highest 4 bits of uint16_t will be masked.
		 * @param count A reference to the length of the instructions (has to point to the first free index)
		 */
		static void load_12bit_immediate(CodeBatch& batch, uint16_t imm, encoding::RiscVRegister dest);

		/**
		 * Loads a 32 bit immediate into the specified register.
		 * The value is sign extended to 64 bit, so be careful when using this method
		 * to ensure that the value is <= 31 bits or negative numbers are desired.
		 * Use load_unsigned_immediate otherwise.
		 * @param batch Store the current riscv-batch.
		 * @param imm The immediate that will be stored. The highest 4 bits of uint16_t will be masked.
		 * @param count A reference to the length of the instructions (has to point to the first free index)
		 * @param optimize When set to true, this method tries to minimize the number of generated instructions.
		 */
		static void load_32bit_immediate(CodeBatch& batch, uint32_t imm, encoding::RiscVRegister dest, bool optimize);

		/**
		 * Loads a 64 bit immediate into the specified register.
		 * The only difference between this method and load_unsigned_immediate is that this one is slightly faster.
		 * Unoptimized this function must generate 8-riscv-instructions. Otherwise update_basic_block will fail.
		 * @param batch Store the current riscv-batch.
		 * @param imm The immediate that will be stored. The highest 4 bits of uint16_t will be masked.
		 * @param count A reference to the length of the instructions (has to point to the first free index)
		 * @param optimize When set to true, this method tries to minimize the number of generated instructions.
		 */
		static void load_64bit_immediate(CodeBatch& batch, uint64_t imm, encoding::RiscVRegister dest, bool optimize);


		/**
		 * Load an immediate of up to 64 bit into the register.
		 * The immediate will not be sign extended (i.e. treated as unsigned) unless it is 64 bit (where sign extension
		 * never happens).
		 * This function always optimizes the number of instructions generated.
		 * @param batch Store the current riscv-batch.
		 * @param imm The immediate that will be loaded.
		 * @param dest The regiser in which the immediate will be loaded.
		 */
		static void load_immediate(CodeBatch& batch, uintptr_t imm, encoding::RiscVRegister dest);

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
			return map_reg(static_cast<fadec::Register>(static_cast<uint8_t>(reg) - 4));
		}

		encoding::RiscVRegister evalute_zero(CodeBatch& batch);

		encoding::RiscVRegister evalute_sign(CodeBatch& batch, encoding::RiscVRegister temp);

		encoding::RiscVRegister evalute_parity(CodeBatch& batch);

		encoding::RiscVRegister evalute_overflow(CodeBatch& batch);

		encoding::RiscVRegister evalute_carry(CodeBatch& batch);

		void update_zero(CodeBatch& batch, bool set);

		void update_zero(CodeBatch& batch, encoding::RiscVRegister va, size_t size);

		void update_sign(CodeBatch& batch, bool set);

		void update_sign(CodeBatch& batch, encoding::RiscVRegister va, size_t size);

		void update_parity(CodeBatch& batch, bool set);

		void update_parity(CodeBatch& batch, encoding::RiscVRegister va, size_t size);

		void update_overflow(CodeBatch& batch, uint16_t index, bool set);

		void update_overflow(CodeBatch& batch, uint16_t index, encoding::RiscVRegister va, encoding::RiscVRegister vb);

		void update_carry(CodeBatch& batch, uint16_t index, bool set);

		void update_carry(CodeBatch& batch, uint16_t index, encoding::RiscVRegister va, encoding::RiscVRegister vb);
	};
}

#endif //OXTRA_INSTRUCTION_H