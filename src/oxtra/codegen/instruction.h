#ifndef OXTRA_INSTRUCTION_H
#define OXTRA_INSTRUCTION_H

#include "oxtra/utils/types.h"
#include "jump-table/jump_table.h"

namespace codegen {
	class Instruction : protected fadec::Instruction {
	public:
		struct Flags {
			static constexpr uint8_t
				none = 0x00,
				carry = 0x01,
				zero = 0x02,
				sign = 0x04,
				overflow = 0x08,
				parity = 0x10,
				all = carry | zero | sign | overflow | parity;
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

	private:
		uint8_t update_flags;
		uint8_t require_flags;
		bool end_of_block;

	protected:
		explicit Instruction(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool eob = false);

	public:
		virtual void generate(CodeBatch& batch) const = 0;

		uint8_t get_require() const;

		uint8_t get_update() const;

		bool get_eob() const;

		void set_update(uint8_t flags);

		void set_eob();

		std::string string() const;

		using fadec::Instruction::get_size;
		using fadec::Instruction::get_address;

	protected:
		/**
		 * Translates a single operand (either register, or memory or immediate value)
		 * @param batch Store the current riscv-batch.
		 * @param inst The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param reg The resulting value will be stored in this register.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @return If this operation was a memory-operation,
		 * 		   the return-register will contain the address (either temp_a, or a base-register)
		 */
		encoding::RiscVRegister translate_operand(CodeBatch& batch, size_t index, encoding::RiscVRegister reg,
												  encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b) const;

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
		void translate_destination(CodeBatch& batch, encoding::RiscVRegister reg, encoding::RiscVRegister address,
								   encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b) const;

		/**
		 * Translates a x86-memory operand into risc-v instructions.
		 * @param batch Store the current riscv-batch.
		 * @param x86_instruction The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @return Returns the register containing the address (either temp_a, or a base-register)
		 */
		encoding::RiscVRegister translate_memory(CodeBatch& batch, size_t index,
												 encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b) const;

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