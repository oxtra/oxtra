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
			/*
			 * IMPORTANT: If a new attribute is needed, append it to the list. Otherwise the assembler
			 * won't function anymore.
			 */
			uint64_t zero_value;
			uint64_t sign_value;
			uint64_t overflow_value[2];
			uint64_t carry_value[2];
			uint16_t overflow_operation;
			uint16_t carry_operation;
			uint8_t sign_size;
			uint8_t parity_value;
			uint16_t __padding0;
			uint64_t overflow_pointer;
			uint64_t carry_pointer;

			static constexpr uint32_t
					flag_info_offset = 0x1F8,
					zero_value_offset = flag_info_offset + sizeof(uint64_t) * 0,
					sign_value_offset = flag_info_offset + sizeof(uint64_t) * 1,
					overflow_values_offset = flag_info_offset + sizeof(uint64_t) * 2,
					carry_values_offset = flag_info_offset + sizeof(uint64_t) * 4,
					overflow_operation_offset = flag_info_offset + sizeof(uint64_t) * 6,
					carry_operation_offset = flag_info_offset + sizeof(uint64_t) * 6 + sizeof(uint16_t),
					sign_size_offset = flag_info_offset + sizeof(uint64_t) * 6 + sizeof(uint16_t) * 2,
					parity_value_offset = flag_info_offset + sizeof(uint64_t) * 6 + sizeof(uint16_t) * 2 + sizeof(uint8_t),
					overflow_ptr_offset = flag_info_offset + sizeof(uint64_t) * 6 + sizeof(uint16_t) * 3 + sizeof(uint8_t) * 2,
					carry_ptr_offset = flag_info_offset + sizeof(uint64_t) * 7 + sizeof(uint16_t) * 3 + sizeof(uint8_t) * 2;
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
		 * Translates a single operand (either register, or memory or immediate value) into the specified register. This method
		 * does not ensure, that upper bits are cleared, or sign extended. Use load_operand if this functionality is required.
		 * @param batch Store the current riscv-batch.
		 * @param inst The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param reg The resulting value will be stored in this register.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @return If this operation was a memory-operation,
		 * 		   the return-register will contain the address (either temp_a, or a base-register)
		 */
		encoding::RiscVRegister
		translate_operand(CodeBatch& batch, size_t index, encoding::RiscVRegister reg, encoding::RiscVRegister temp_a,
						  encoding::RiscVRegister temp_b) const;

		/**
		 * This method can be used to load a part of a register / value (e.g. a byte if op_size is a byte) and store it in a
		 * register. Optionally, this value will be sign extended. Use this method when you do not change the operand but require
		 * the plain value to be stored for further calculation.
		 *
		 * Contrary to the normal translate_operand method, this method can sign extend and ensures
		 * that the destination register will only have the correct bits set. Further, the destination register itself will be
		 * returned instead of the memory address. This can be used so that 64 bit registers do not have to be moved.
		 * @param index The index of
		 * @param reg The register that will be used to move the register to (if required). 64 bit registers do not have to be moved.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @param sign_extend If set to true, the value will be sign extended. This variable does not influence immediates.
		 * @return The register where the operand has been loaded to (if it is e.g. just RAX, just RAX will be returned).
		 */
		encoding::RiscVRegister load_operand(codegen::CodeBatch& batch, size_t index,
											 encoding::RiscVRegister reg, encoding::RiscVRegister temp_a,
											 encoding::RiscVRegister temp_b, bool sign_extend) const;

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
		 * @return Returns the register containing the address (either temp_a, a base-register/index-register or zero)
		 */
		encoding::RiscVRegister
		translate_memory(CodeBatch& batch, size_t index, encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b) const;

		/**
		 * The value of the zero flag is returned in t4 = 0/1. Registers t4, t5, t6 may be modified.
		 */
		void evaluate_zero(CodeBatch& batch) const;

		/**
		 * The value of the sign flag is returned in t4 = 0/1. Registers t4, t5, t6 may be modified.
		 */
		void evaluate_sign(CodeBatch& batch, encoding::RiscVRegister temp) const;

		/**
		 * The value of the parity flag is returned in t4 = 0/1. Registers t4, t5, t6 may be modified.
		 */
		void evaluate_parity(CodeBatch& batch, encoding::RiscVRegister temp) const;

		/**
		 * The value of the overflow flag is returned in t4 = 0/1. Registers t4, t5, t6 may be modified.
		 */
		void evaluate_overflow(CodeBatch& batch) const;

		/**
		 * The value of the carry flag is returned in t4 = 0/1. Registers t4, t5, t6 may be modified.
		 */
		void evaluate_carry(CodeBatch& batch) const;

		void update_zero(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const;

		void update_zero(CodeBatch& batch, encoding::RiscVRegister va, uint8_t size) const;

		void update_sign(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const;

		void update_sign(CodeBatch& batch, encoding::RiscVRegister va, uint8_t size, encoding::RiscVRegister temp) const;

		void update_parity(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const;

		void update_parity(CodeBatch& batch, encoding::RiscVRegister va) const;

		void update_overflow(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const;

		void update_overflow(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va, encoding::RiscVRegister vb,
							 encoding::RiscVRegister temp) const;

		void update_overflow(CodeBatch& batch, encoding::RiscVRegister entry,
				encoding::RiscVRegister va, encoding::RiscVRegister vb) const;

		void update_carry(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const;

		void update_carry(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va, encoding::RiscVRegister vb,
						  encoding::RiscVRegister temp) const;

		void update_carry(CodeBatch& batch, encoding::RiscVRegister entry,
							 encoding::RiscVRegister va, encoding::RiscVRegister vb) const;

		void update_carry_unsupported(CodeBatch& batch, const char* instruction, encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the returnvalue in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void update_carry_high_level(CodeBatch& batch, uintptr_t(*callback)(void*),
				encoding::RiscVRegister temp) const;

		void update_overflow_unsupported(CodeBatch& batch, const char* instruction, encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the returnvalue in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void update_overflow_high_level(CodeBatch& batch, uintptr_t(*callback)(void*),
				encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the returnvalue in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void call_high_level(CodeBatch& batch, uintptr_t(*callback)(void*)) const;
	};
}

#endif //OXTRA_INSTRUCTION_H