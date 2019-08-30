#ifndef OXTRA_FLAGS_H
#define OXTRA_FLAGS_H

#include "oxtra/codegen/encoding/encoding.h"

namespace codegen::flags {
	namespace Group {
		using Type = uint32_t;

		static constexpr Type none = 0x0000u;
		static constexpr Type require_zero = 0x0001u;
		static constexpr Type require_sign = 0x0002u;
		static constexpr Type require_carry = 0x0004u;
		static constexpr Type require_overflow = 0x0008u;
		static constexpr Type require_parity = 0x0010u;
		static constexpr Type require_all = require_zero | require_sign | require_carry | require_overflow | require_parity;

		static constexpr Type update_zero = 0x0100u;
		static constexpr Type update_sign = 0x0200u;
		static constexpr Type update_carry = 0x0400u;
		static constexpr Type update_overflow = 0x0800u;
		static constexpr Type update_parity = 0x1000u;
		static constexpr Type update_all = update_zero | update_sign | update_carry | update_overflow | update_parity;
		static constexpr Type end_of_block = 0x8000u | require_all;
		static constexpr Type error = 0xffffffffu;
		static constexpr Type require_to_update_lshift = 8u;
	}

	enum class FlagOperation : uint8_t {
		add,
		sub,
		//...
	};

	/**
	 * Stores information about the last operation that changed specific flags.
	 */
	struct FlagUpdateInfo {
		/*
		 * zero flag
		 * store qword ptr 0
		 * store value
		 */
		uintptr_t zf_value; // 0x00

		/*
		 * sign flag
		 * store value
		 * store size
		 */
		uintptr_t sf_value; // 0x08

		/*
		 * carry flag
		 * store operation type
		 * store operand size
		 * store sources/results
		 */
		uintptr_t cf_values[2]; // 0x10

		/*
		 * overflow flag
		 * store operation type
		 * store operand size
		 * store sources/results
		 */
		uintptr_t of_values[2]; // 0x20

		/*
		 * we want better padding
		 */
		uint8_t sf_size; // 0x30
		uint8_t pf_value; // 0x31
		uint8_t cf_operation; // 0x32
		uint8_t cf_size; // 0x33
		uint8_t of_operation; // 0x34
		uint8_t of_size; // 0x35

		static constexpr uint32_t
				flag_info_offset = 0x1F8,
				zf_value_offset = flag_info_offset + 0x00,
				sf_value_offset = flag_info_offset + 0x08,
				cf_values_offset = flag_info_offset + 0x10,
				of_values_offset = flag_info_offset + 0x20,
				sf_size_offset = flag_info_offset + 0x30,
				pf_value_offset = flag_info_offset + 0x31,
				cf_operation_offset = flag_info_offset + 0x32,
				cf_size_offset = flag_info_offset + 0x33,
				of_operation_offset = flag_info_offset + 0x34,
				of_size_offset = flag_info_offset + 0x35;
	};

	/**
	 * Generate the code required to update the zero flag. The register will stay unchanged.
	 * Call this method for every instruction, which updates the zero flag.
	 * @param reg Register which contains the result-value.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_zero_flag(encoding::RiscVRegister reg, uint8_t reg_size,
						  utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Evaluate the zero flag. Call this method for every instruction, which requires the value of the zero flag.
	 * @param invert If true, branch if the flag is cleared.
	 * @param offset The number of bytes that will be skipped if the flag is set.
	 * @param temp A temporary that might be changed.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void evaluate_zero_flag(bool invert, uint16_t offset, encoding::RiscVRegister temp, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Update the sign-flag from the result-register. The register will stay unchanged.
	 * Call this method for every instruction, which updates the sign flag.
	 * @param reg Register which contains the result-value.
	 * @param temp A temporary that might be changed.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_sign_flag(encoding::RiscVRegister reg, encoding::RiscVRegister temp, uint8_t reg_size,
						  utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Evaluate the sign flag. Call this method for every instruction, which requires the value of the sign flag.
	 * @param invert If true, branch if the flag is cleared.
	 * @param offset The number of bytes that will be skipped if the flag is set.
	 * @param temp_a A temporary that might be changed.
	 * @param temp_b A temporary that might be changed.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void evaluate_sign_flag(bool invert, uint16_t offset, encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b,
							utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Update the parity-flag from the result-register. The register will stay unchanged.
	 * Call this method for every instruction, which updates the parity flag.
	 * @param reg Register which contains the result-value.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_parity_flag(encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Evaluate the parity flag. Call this method for every instruction, which requires the value of the parity flag.
	 * @param invert If true, branch if the flag is cleared.
	 * @param offset The number of bytes that will be skipped if the flag is set.
	 * @param temp_a A temporary that might be changed.
	 * @param temp_b A temporary that might be changed.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void evaluate_parity_flag(bool invert, uint16_t offset, encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b,
							  utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Update the carry-flag from the registers {reg_a, reg_b}. The registers will stay unchanged.
	 * Call this method for every instruction, which updates the carry flag.
	 * @param reg_a Operation dependent register.
	 * @param reg_b Operation dependent register.
	 * @param temp A temporary that might be changed.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param operation The operation that updated the flags.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_carry_flag(encoding::RiscVRegister reg_a, encoding::RiscVRegister reg_b, encoding::RiscVRegister temp,
						   uint8_t reg_size, FlagOperation operation, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Evaluate the carry flag. Call this method for every instruction, which requires the value of the carry flag.
	 * @param invert If true, branch if the flag is cleared.
	 * @param reg This register will be zero if the flag is cleared and not equal to zero otherwise.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void evaluate_carry_flag(bool invert, encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Update the overflow-flag from the registers {reg_a, reg_b}. The registers will stay unchanged.
	 * Call this method for every instruction, which updates the overflow flag.
	 * @param reg_a Operation dependent register.
	 * @param reg_b Operation dependent register.
	 * @param temp A temporary that might be changed.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param operation The operation that updated the flags.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_overflow_flag(encoding::RiscVRegister reg_a, encoding::RiscVRegister reg_b, encoding::RiscVRegister temp,
							  uint8_t reg_size, FlagOperation operation, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Evaluate the overflow flag. Call this method for every instruction, which requires the value of the overflow flag.
	 * @param invert If true, branch if the flag is cleared.
	 * @param reg This register will be zero if the flag is cleared and not equal to zero otherwise.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void evaluate_overflow_flag(bool invert, encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count);
}

#endif //OXTRA_FLAGS_H
