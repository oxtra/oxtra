#ifndef OXTRA_INSTRUCTION_H
#define OXTRA_INSTRUCTION_H

#include "oxtra/utils/types.h"
#include "oxtra/codegen/jump-table/jump_table.h"
#include "oxtra/dispatcher/execution_context.h"

/*
 * IMPORTANT: At any point throughout the execution, the code must never rely on a certain
 * value in the register ra, as its contents might change due to a debugger or other high-level-code.
 *
 * The c_callback_t-type describes a callback, which you can use in your code to call a given function in
 * high-level-code. Within the callback, ra and t4's contents will have changed. But the callback's returned
 * uintptr_t will be placed into t4, which is very convenient for high-level-flag-evaluation.
 */

namespace codegen {
	class Instruction : protected fadec::Instruction {
	private:
		uint8_t update_flags;
		uint8_t require_flags;
		bool end_of_block;

	protected:
		explicit Instruction(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool eob = false);

	public:
		using c_callback_t = uintptr_t(*)(dispatcher::ExecutionContext*);

		virtual void generate(CodeBatch& batch) const = 0;

		uint8_t get_require() const;

		uint8_t get_update() const;

		bool get_eob() const;

		void set_update(uint8_t flags);

		std::string string() const;

		using fadec::Instruction::get_size;
		using fadec::Instruction::get_address;

	protected:
		/**
		 * Translates a single operand (either register, or memory or immediate value) into the specified register.
		 * @param inst The x86 instruction object.
		 * @param index operand-index of instruction.
		 * @param address If the operation was a memory-operation, this register will contain the address (for optimizations).
		 * 		  Either: zero, temp_b or one of the mapped registers. (can be Null)
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @param modifiable If true, the function will ensure to load the value into a temporary register (unless destination is true).
		 * @param full_load If true, the register will only contain the value loaded. Otherwise the upper bits might still contain other contents.
		 * @param sign_extend If full_load is true, this attribute allows to indicate whether or not the value should be stored as sign-extended or not.
		 * @param destination indicates that this operand will later be used as destination (will optimize in conjunction with modifiable).
		 * 	      If set to true, either call translate_destination, or clear the upper 32bits on 32-bit registers. (if returned)
		 * @return The register which contains the value.
		 * 		   Either: temp_a or one of the mapped registers (or zero, if the operand-type is unknown).
		 */
		encoding::RiscVRegister translate_operand(CodeBatch& batch, size_t index, encoding::RiscVRegister* address,
												  encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b,
												  bool modifiable, bool full_load, bool sign_extend, bool destination) const;

		/**
		 * Writes the value in the register to the destination-operand of the instruction.
		 * The register will be preserved. Undefined behavior for reg == operand->reg.
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
		 * @param index operand-index of instruction.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @return Returns the register containing the address (either temp_a, a base-register/index-register or zero)
		 */
		encoding::RiscVRegister translate_memory(CodeBatch& batch, size_t index, encoding::RiscVRegister temp_a,
												 encoding::RiscVRegister temp_b) const;

		/**
		 * Reads from memory given an x86-memory operand.
		 * @param index operand-index of instruction.
		 * @param dest The register which will contain the value.
		 * @param sign_extended The result should be sign-extended.
		 * @return Returns the register containing the partial address (either temp_a, a base-register/index-register or zero)
		 * 		   DONT USE THIS ADDRESS YOURSELF.
		 * 		   Use write_to_memory, as it might be, that the displacement has not been added to the register.
		 */
		encoding::RiscVRegister read_from_memory(CodeBatch& batch, size_t index, encoding::RiscVRegister dest,
												 encoding::RiscVRegister temp, bool sign_extended) const;

		/**
		 * Writes a register to a given x86-memory operand.
		 * The content of the register will be preserved.
		 * @param index operand-index of instruction.
		 * @param src The register containing the value.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @param address A register containing the partial address (must be generated by read_from memory, as
		 * 		  the function expects certain optimizations, if the address is not the zero-register)
		 */
		void write_to_memory(CodeBatch& batch, size_t index, encoding::RiscVRegister src, encoding::RiscVRegister temp_a,
							 encoding::RiscVRegister temp_b, encoding::RiscVRegister address) const;

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

		/**
		 * Updates the first overflow value with the value in register va and sets the operation.
		 */
		void update_overflow_single(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va,
				encoding::RiscVRegister temp) const;

		/**
		 * Updates the second overflow value with the value in register vb.
		 */
		void update_overflow_single(CodeBatch& batch, encoding::RiscVRegister vb) const;

		void update_overflow(CodeBatch& batch, encoding::RiscVRegister entry,
							 encoding::RiscVRegister va, encoding::RiscVRegister vb) const;

		void update_carry(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const;

		void update_carry(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va, encoding::RiscVRegister vb,
						  encoding::RiscVRegister temp) const;

		/**
		 * Updates the first carry value with the value in register va nd sets the operation.
		 */
		void update_carry_single(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va,
				encoding::RiscVRegister temp) const;

		/**
		 * Updates the second carry value with the value in register vb.
		 */
		void update_carry_single(CodeBatch& batch, encoding::RiscVRegister vb) const;

		void update_carry(CodeBatch& batch, encoding::RiscVRegister entry,
						  encoding::RiscVRegister va, encoding::RiscVRegister vb) const;

		void update_carry_unsupported(CodeBatch& batch, const char* instruction, encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the returnvalue in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void update_carry_high_level(CodeBatch& batch, c_callback_t callback,
									 encoding::RiscVRegister temp) const;

		void update_overflow_unsupported(CodeBatch& batch, const char* instruction, encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the returnvalue in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void update_overflow_high_level(CodeBatch& batch, c_callback_t callback,
										encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the returnvalue in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void call_high_level(CodeBatch& batch, c_callback_t callback) const;
	};
}

#endif //OXTRA_INSTRUCTION_H