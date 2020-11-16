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

		/**
		 * Returns the direct branch address or 0 if there is none.
		 */
		virtual uintptr_t branch_address() const;

		/**
		 * Returns the number of possible control flow changes.
		 * 0: normal instructions
		 * 1: jmp, call..
		 * 2: jcc
		 */
		virtual uint8_t control_flow_dimension() const;

		uint8_t get_update() const;

		bool get_eob() const;

		void set_update(uint8_t flags);

		std::string string() const;

		using fadec::Instruction::get_size;
		using fadec::Instruction::get_address;

	protected:
		/**
		 * Translates a single operand (either register, or memory or immediate value) into the specified register.
		 * @param op The operand that's translated.
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
		encoding::RiscVRegister translate_operand(CodeBatch& batch, const fadec::Operand& op, encoding::RiscVRegister* address,
												  encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b,
												  bool modifiable, bool full_load, bool sign_extend, bool destination) const;

		/**
		 * Writes the value in the register to the destination-operand of the instruction.
		 * The register will be preserved. Undefined behavior for reg == operand->reg.
		 * @param reg This value will be written to the destination.
		 * @param address If the destination is a memory address, this address will be used as destination.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 */
		void translate_destination(CodeBatch& batch, encoding::RiscVRegister reg, encoding::RiscVRegister address,
								   encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b) const;

		/**
		 * Writes the resulting address of a x86-memory operand into a risc-v register.
		 * @param op The operand that contains the memory addressing.
		 * @param dst The destination register. Might not be used, unless force_dst is true.
		 * @param temp_a A temporary register.
		 * @param temp_b A temporary register.
		 * @param force_dst Forces the address to be written to the dst register.
		 * @return The register that contains the address.
		 */
		encoding::RiscVRegister translate_memory(CodeBatch& batch, const fadec::Operand& op, encoding::RiscVRegister dst,
												 encoding::RiscVRegister temp, bool force_dst = false) const;

		/**
		 * Reads from memory given an x86-memory operand.
		 * @param op The operand that contains the memory address to be read from.
		 * @param dest The register which will contain the value.
		 * @param sign_extended The result should be sign-extended.
		 * @return Returns the register containing the partial address (either temp_a, a base-register/index-register or zero)
		 * 		   DONT USE THIS ADDRESS YOURSELF.
		 * 		   Use write_to_memory, as it might be, that the displacement has not been added to the register.
		 */
		encoding::RiscVRegister read_from_memory(CodeBatch& batch, const fadec::Operand& op, encoding::RiscVRegister dest,
												 encoding::RiscVRegister temp, bool sign_extended) const;

		/**
		 * Writes a register to a given x86-memory operand.
		 * The content of the register will be preserved.
		 * @param op The operand that contains the memory address to be written to.
		 * @param src The register containing the value.
		 * @param temp_a A temporary that might be changed.
		 * @param temp_b A temporary that might be changed.
		 * @param address A register containing the partial address (must be generated by read_from memory, as
		 * 		  the function expects certain optimizations, if the address is not the zero-register)
		 */
		void write_to_memory(CodeBatch& batch, const fadec::Operand& op, encoding::RiscVRegister src,
							 encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b,
							 encoding::RiscVRegister address) const;

		/**
		 * Handles the segment overrides in addressing.
		 * @param base Reference to the register that holds the current address of the operand. This register might be modified.
		 */
		void handle_segment_override(CodeBatch& batch, encoding::RiscVRegister& base, encoding::RiscVRegister temp) const;

		/**
		 * The value of the zero flag is returned in dest = 0/1.
		 */
		void evaluate_zero(CodeBatch& batch, encoding::RiscVRegister dest) const;

		/**
		 * The value of the sign flag is returned in dest = 0/1.
		 */
		void evaluate_sign(CodeBatch& batch, encoding::RiscVRegister dest, encoding::RiscVRegister temp) const;

		/**
		 * The value of the parity flag is returned in dest = 0/1.
		 */
		void evaluate_parity(CodeBatch& batch, encoding::RiscVRegister dest, encoding::RiscVRegister temp) const;

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

		/**
		 * Updates the overflow flag information in the execution context.
		 * The parameters may be null if no information should be updated.
		 * The temp register is required if a jump table entry should be set.
		 */
		void update_overflow(CodeBatch& batch, const jump_table::Entry* entry, const encoding::RiscVRegister* va,
							 const encoding::RiscVRegister* vb,
							 encoding::RiscVRegister temp = encoding::RiscVRegister::zero) const;

		void update_overflow(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const;

		void update_overflow(CodeBatch& batch, encoding::RiscVRegister entry,
							 encoding::RiscVRegister va, encoding::RiscVRegister vb) const;

		/**
		 * Updates the carry flag information in the execution context.
		 * The parameters may be null if no information should be updated.
		 * The temp register is required if a jump table entry should be set.
		 */
		void update_carry(CodeBatch& batch, const jump_table::Entry* entry, const encoding::RiscVRegister* va,
						  const encoding::RiscVRegister* vb,
						  encoding::RiscVRegister temp = encoding::RiscVRegister::zero) const;

		void update_carry(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const;

		/**
		 * Updates the carry operation with the entry specified in a register.
		 * Mostly useful to set carry_clear and carry_set.
		 */
		void update_carry(CodeBatch& batch, encoding::RiscVRegister entry) const;

		void update_carry(CodeBatch& batch, encoding::RiscVRegister entry,
						  encoding::RiscVRegister va, encoding::RiscVRegister vb) const;

		void update_carry_unsupported(CodeBatch& batch, const char* instruction, encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the return value in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void update_carry_high_level(CodeBatch& batch, c_callback_t callback,
									 encoding::RiscVRegister temp) const;

		void update_overflow_unsupported(CodeBatch& batch, const char* instruction, encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the return value in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void update_overflow_high_level(CodeBatch& batch, c_callback_t callback,
										encoding::RiscVRegister temp) const;

		/*
		 * Invokes the callback with ExecutionContext* as argument and stores the return value in t4.
		 * t4 Value will be overriden before reaching the callback
		 */
		void call_high_level(CodeBatch& batch, c_callback_t callback) const;
	};
}

#endif //OXTRA_INSTRUCTION_H