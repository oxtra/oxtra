#ifndef OXTRA_HELPER_H
#define OXTRA_HELPER_H

#include "code_batch.h"
#include "jump-table/jump_table.h"

namespace codegen::helper {
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

	struct RegisterAccess {
		static constexpr uint8_t QWORD = 8;
		static constexpr uint8_t DWORD = 4;
		static constexpr uint8_t WORD = 2;
		static constexpr uint8_t LBYTE = 1;
		static constexpr uint8_t HBYTE = 0;
	};

	/**
	 * Writes a register with x86-style sub-manipulation to an existing register WITHOUT
	 * invalidating the rest of the value. Undefined behavior if src == dest.
	 * The source-register will be preserved.
	 * @param dest Register to be changed.
	 * @param src Register to write.
	 * @param access The Operand-size (8=QWORD, 4=DWORD, 2=WORD, 1=LBYTE, 0=HBYTE)
	 * @param cleared If true the upper bits of the source register are expected to be 0.
	 */
	void move_to_register(CodeBatch& batch, encoding::RiscVRegister dest, encoding::RiscVRegister src, uint8_t access,
						  encoding::RiscVRegister temp, bool cleared);

	/**
	 * Reads a register with x86-style sub-manipulation into a temporary register.
	 * Undefined behavior if src == temp.
	 * @param src The source to copy.
	 * @param access The Operand-size (8=QWORD, 4=DWORD, 2=WORD, 1=LBYTE, 0=HBYTE)
	 * @param modifiable The resulting register must be modifiable (Enforces returning the temp register).
	 * @param full_load In case of an access < 8, the resulting register will have the upper bits cleared.
	 * @param sign_extend In case of a full_load, the resulting register will be sign-extended or not.
	 * @return The register which contains the value (either the source itself or the temp register).
	 */
	encoding::RiscVRegister load_from_register(CodeBatch& batch, encoding::RiscVRegister src, uint8_t access,
											   encoding::RiscVRegister temp, bool modifiable, bool full_load, bool sign_extend);

	/**
	 * Load an immediate of up to 64 bit into the register.
	 * This function always optimizes the number of instructions generated.
	 * @param imm The immediate that will be loaded.
	 * @param dest The register in which the immediate will be loaded.
	 */
	void load_immediate(CodeBatch& batch, uintptr_t imm, encoding::RiscVRegister dest);

	/**
	 * Loads an address into a riscv register. Unconditionally generates 8 instruction.
	 */
	void load_address(CodeBatch& batch, uintptr_t ptr, encoding::RiscVRegister dest);

	/**
	 * Appends a jump into reroute_static.
	 * @param ptr The address of the next block.
	 */
	void append_eob(CodeBatch& batch, uintptr_t ptr);

	/**
	 * Appends a jump into reroute_dynamic.
	 * @param reg The register that contains the address of the next block.
	 */
	void append_eob(CodeBatch& batch, encoding::RiscVRegister reg);

	/**
	 * Sign extend a given source register (which will not be modified) into a destination register.
	 * @param batch Store the current riscv-batch.
	 * @param dest The register where the sign extended value will be stored.
	 * @param src The register that contains the value that will be sign extended.
	 * @param byte The number of bytes that are stored in the given register (e.g. EAX: 4, AX: 2).
	 */
	void
	sign_extend_register(codegen::CodeBatch& batch, encoding::RiscVRegister dest, encoding::RiscVRegister src, size_t byte);

	/**
	 * Calculates the jump table indices for the carry and overflow flags based on the size of the operand.
	 * @param carry The carry index.
	 * @param overflow The overflow index.
	 * @param size The operand size.
	 * @return The pair consisting of {new_carry_index, new_overflow_index}.
	 */
	std::pair<jump_table::Entry, jump_table::Entry>
	calculate_entries(jump_table::Entry carry, jump_table::Entry overflow, uint8_t size);

	/**
	 * Returns the riscv-register, which maps to the x86-register.
	 * @param reg The x86-register.
	 * @return The riscv-register.
	 */
	constexpr encoding::RiscVRegister map_reg(const fadec::Register reg) {
		constexpr encoding::RiscVRegister register_mapping[] = {
				encoding::RiscVRegister::rax,
				encoding::RiscVRegister::rcx,
				encoding::RiscVRegister::rdx,
				encoding::RiscVRegister::rbx,
				encoding::RiscVRegister::rsp,
				encoding::RiscVRegister::rbp,
				encoding::RiscVRegister::rsi,
				encoding::RiscVRegister::rdi,
				encoding::RiscVRegister::r8,
				encoding::RiscVRegister::r9,
				encoding::RiscVRegister::r10,
				encoding::RiscVRegister::r11,
				encoding::RiscVRegister::r12,
				encoding::RiscVRegister::r13,
				encoding::RiscVRegister::r14,
				encoding::RiscVRegister::r15
		};

		return register_mapping[static_cast<uint8_t>(reg)];
	}

	/**
	 * Maps the x86 gph register to it's base's riscv register.
	 * @param reg The x86-register.
	 * @return The riscv-register.
	 */
	constexpr encoding::RiscVRegister map_reg_high(const fadec::Register reg) {
		return map_reg(static_cast<fadec::Register>(static_cast<uint8_t>(reg) - 4));
	}
}

#endif //OXTRA_HELPER_H
