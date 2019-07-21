#ifndef OXTRA_FLAGS_H
#define OXTRA_FLAGS_H

#include "encoding/encoding.h"

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
		uintptr_t zf_value;

		/*
		 * sign flag
		 * store value
		 * store size
		 */
		uintptr_t sf_value;

		/*
		 * parity flag
		 * store qword ptr 0
		 * store value
		 */
		uintptr_t pf_value;

		/*
		 * carry flag
		 * store operation type
		 * store operand size
		 * store sources/results
		 */
		uintptr_t cf_value[2];

		/*
		 * overflow flag
		 * store operation type
		 * store operand size
		 * store sources/results
		 */
		uintptr_t of_values[2];

		/*
		 * we want better padding
		 */
		uint8_t sf_size;
		uint8_t cf_operation;
		uint8_t cf_size;
		uint8_t of_operation;
		uint8_t of_size;
	};

	/**
	 * Update the zero-flag from the result-register. The register will stay unchanged.
	 * @param reg Register which contains the result-value.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_zero_flag(encoding::RiscVRegister reg, uint8_t reg_size,
						  utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Update the sign-flag from the result-register. The register will stay unchanged.
	 * @param reg Register which contains the result-value.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param temp A temporary that might be changed.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_sign_flag(encoding::RiscVRegister reg, uint8_t reg_size, encoding::RiscVRegister temp,
						  utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Update the parity-flag from the result-register. The register will stay unchanged.
	 * @param reg Register which contains the result-value.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_parity_flag(encoding::RiscVRegister reg, uint8_t reg_size,
							utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Update the carry-flag from the registers {reg_a, reg_b}. The registers will stay unchanged.
	 * @param reg_a Operation dependent register.
	 * @param reg_b Operation dependent register.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param operation The operation that updated the flags.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_carry_flag(encoding::RiscVRegister reg_a, encoding::RiscVRegister reg_b, uint8_t reg_size,
						   FlagOperation operation, utils::riscv_instruction_t* riscv, size_t& count);

	/**
	 * Update the overflow-flag from the registers {reg_a, reg_b}. The registers will stay unchanged.
	 * @param reg_a Operation dependent register.
	 * @param reg_b Operation dependent register.
	 * @param reg_size Operand-size of the register (8,4,2,1).
	 * @param operation The operation that updated the flags.
	 * @param riscv The pointer to the generated riscv instructions.
	 * @param count The current length of the riscv instructions (i.e. the index of the next free position).
	 */
	void update_overflow_flag(encoding::RiscVRegister reg_a, encoding::RiscVRegister reg_b, uint8_t reg_size,
							  FlagOperation operation, utils::riscv_instruction_t* riscv, size_t& count);
}

#endif //OXTRA_FLAGS_H
