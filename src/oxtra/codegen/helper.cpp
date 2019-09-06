#include "oxtra/codegen/helper.h"
#include "oxtra/codegen/encoding/encoding.h"
#include <spdlog/spdlog.h>
#include <oxtra/dispatcher/dispatcher.h>

using namespace encoding;

codegen::helper::RegisterAccess codegen::helper::operand_to_register_access(const fadec::Operand& operand)  {
	const auto op_size = operand.get_size();
	if (op_size == 8) return RegisterAccess::QWORD;
	if (op_size == 4) return RegisterAccess::DWORD;
	if (op_size == 2) return RegisterAccess::WORD;

	return operand.get_register_type() == fadec::RegisterType::gph ? RegisterAccess::HBYTE : RegisterAccess::LBYTE;
}

RiscVRegister codegen::helper::load_register(codegen::CodeBatch& batch, RiscVRegister src, RiscVRegister dest,
											 RegisterAccess access, bool sign_extend) {
	uint8_t operand_size = 1;
	if (access == RegisterAccess::QWORD) {
		operand_size = 8;
	} else if (access == RegisterAccess::DWORD) {
		operand_size = 4;
	} else if (access == RegisterAccess::WORD) {
		operand_size = 2;
	}

	uint8_t shamt = 64 - operand_size * 8;
	if (access == RegisterAccess::HBYTE) shamt -= 8;

	if (shamt == 0) {
		return src;
	} else {
		if (operand_size == 4 && sign_extend) {
			batch += encoding::ADDW(dest, src, RiscVRegister::zero);
		} else {
			batch += encoding::SLLI(dest, src, shamt);

			if (access == RegisterAccess::HBYTE) shamt += 8;

			batch += (sign_extend ? encoding::SRAI : encoding::SRLI)(dest, dest, shamt);
		}
	}

	return dest;
}

void codegen::helper::move_to_register(CodeBatch& batch, RiscVRegister dest, RiscVRegister src, RegisterAccess access,
									   RiscVRegister temp, bool cleared) {
	switch (access) {
		case RegisterAccess::QWORD:
			batch += encoding::MV(dest, src);
			return;
		case RegisterAccess::DWORD:
			load_register(batch, src, dest, access, false);
			return;
		case RegisterAccess::WORD:
			// check if the upper source-register has to be cleared
			if (!cleared) {
				batch += encoding::XOR(temp, src, dest);
				load_register(batch, temp, temp, access, false);
				batch += encoding::XOR(dest, temp, dest);
			} else {
				// clear the lower bits of the destination-register by shifting
				batch += encoding::SRLI(dest, dest, 16);
				batch += encoding::SLLI(dest, dest, 16);
				batch += encoding::OR(dest, dest, src);
			}
			return;
		case RegisterAccess::LBYTE:
			// clear the destination
			batch += encoding::ANDI(dest, dest, -0x100);

			// extract the lower bits of the source-register and merge the registers
			if (!cleared)
				batch += encoding::ANDI(temp, src, 0xff);
			batch += encoding::OR(dest, dest, cleared ? src : temp);
			return;
		case RegisterAccess::HBYTE:
			// move the 8 bits of the destination-register down and xor them with the source
			batch += encoding::SRLI(temp, dest, 8);
			batch += encoding::XOR(temp, temp, src);

			load_register(batch, temp, temp, access, false);

			// xor the temporary register to the destination
			batch += encoding::XOR(dest, temp, dest);
			return;
	}
}

void codegen::helper::load_immediate(CodeBatch& batch, uintptr_t imm, encoding::RiscVRegister dest) {

	/* Number-structure: 00 00 01 11 22 23 33 44 */

	// create the member-stack
	uint32_t packages[5] = {0};
	bool increase[5] = {true, true, true, true, true};
	uint8_t shifts[4] = {0};
	uint8_t index = 0;
	int8_t current_bit = 63u;
	bool lui_used = false;

	// enter the main encoding-loop
	while (true) {
		// count the number of equal bits in a row beginning at the current bit
		uint8_t upper_value = (imm >> static_cast<uint8_t>(current_bit)) & 0x01u;
		uint8_t equal_digits = 1;
		for (current_bit--; current_bit >= 0; current_bit--) {
			if (((imm >> static_cast<uint8_t>(current_bit)) & 0x01u) != upper_value)
				break;
			equal_digits++;
		}

		/* Compute the number of digits of the current number.
		 * This number is based on the number of equal bits in the beginning
		 * as well as the number of bits after the first change and
		 * the fact whether or not LUI has already been used or even is usable here.
		 * the equal_bits, current_bit and digits are updated accordingly. */
		uint8_t digits = current_bit + 2;
		equal_digits--;
		if (digits < 12) {
			digits += equal_digits;
			if (digits > 12) {
				equal_digits = digits - 12;
				digits = 12;
			} else
				equal_digits = 0;
			current_bit = -1;
		} else if (digits > 12) {
			if (digits >= 32 && index == 0) {
				current_bit = digits - 21;
				digits = 20;
			} else if (digits + equal_digits >= 32 && index == 0) {
				digits += equal_digits;
				if (digits > 32) {
					equal_digits = digits - 32;
					digits = 32;
				} else
					equal_digits = 0;
				digits -= 12;
				current_bit = 11;
			} else {
				current_bit = digits - 13;
				digits = 12;
			}
		} else
			current_bit -= digits - 1;

		/* In case of the number with multiple zeros in front of it, it must not
		 * be affected by an addition of a negative number. Thus, if the lowest bit
		 * of the number is set, or the two others before, we have to move the entire
		 * digit up one, to have two zero's at its beginning. Otherwise the sign-extension
		 * might lead to all of the zeros in front of the number to become one's. */
		if (upper_value == 0 && equal_digits > 0 && current_bit >= 1) {
			if (((imm >> static_cast<uint8_t>(current_bit + 1)) & 0x01u) &&
				(((imm >> static_cast<uint8_t>(current_bit)) & 0x01u) ||
				 ((imm >> static_cast<uint8_t>(current_bit - 1)) & 0x01u))) {
				equal_digits--;
				current_bit++;
			}
		}

		// store the number to the package-array and check if its parent can be increased, or not
		packages[index] = (imm >> static_cast<uint8_t>(current_bit + 1)) & (0xffffffffffffffffull >> (64u - digits));
		if (index == 0 && digits == 20)
			lui_used = true;
		if (equal_digits > 0 && index > 0 && upper_value == 0)
			increase[index - 1] = false;

		// check if the number requires the parent to be increased, as it will decrease the parent on load
		if (packages[index] >= 0x800 && index > 0) {
			uint8_t temp = index - 1;
			while (packages[temp + 1] >= 0x800) {
				if (!increase[temp])
					break;
				increase[temp] = false;
				packages[temp]++;
				if (temp == 0)
					break;
				temp--;
			}
		}
		index++;

		// compute the shifts and store them in the shift-array
		if (index > 1) {
			if (lui_used && index == 2)
				shifts[index - 2] = digits + equal_digits - 12;
			else
				shifts[index - 2] = digits + equal_digits;
		}

		// check if all of the bits have been processed
		if (current_bit < 0)
			break;
	}

	// create the instructions
	encoding::RiscVRegister reg = encoding::RiscVRegister::zero;
	for (uint8_t i = 0; i < index; i++) {
		if (i == 0 && lui_used) {
			reg = dest;
			batch += encoding::LUI(dest, packages[i]);
		} else if (packages[i] > 0 || i == 0) {
			batch += encoding::ADDI(dest, reg, packages[i]);
			reg = dest;
		}
		if (i + 1 < index && shifts[i] > 0) {
			batch += encoding::SLLI(dest, reg, shifts[i]);
			reg = dest;
		}
	}
}

void codegen::helper::load_address(codegen::CodeBatch& batch, uintptr_t ptr, encoding::RiscVRegister dest) {
	/* Number-structure: 00 00 01 11 22 23 33 44 */

	// extract the number-packages
	uint32_t packages[5];
	packages[0] = static_cast<uint32_t>(ptr & 0x00000000000000ffu);
	packages[1] = static_cast<uint32_t>((ptr & 0x00000000000fff00ul) >> 8u);
	packages[2] = static_cast<uint32_t>((ptr & 0x00000000fff00000ul) >> 20u);
	packages[3] = static_cast<uint32_t>((ptr & 0x00000fff00000000ull) >> 32u);
	packages[4] = static_cast<uint32_t>((ptr & 0xfffff00000000000ull) >> 44u);
	constexpr uint32_t high_bit[5] = {0x00000000u, 0x00000800u, 0x00000800u, 0x00000800u, 0x00080000u};

	// in case of negative numbers, increase the next component
	for (uint8_t i = 1; i < 4; i++) {
		if (packages[i] >= high_bit[i])
			packages[i + 1]++;
	}

	// generate the code to set the immediates
	batch += encoding::LUI(dest, packages[4]);
	for (size_t i = 4; i > 0; i--) {
		batch += encoding::ADDI(dest, dest, packages[i - 1]);
		if (i > 1)
			batch += encoding::SLLI(dest, dest, i == 2 ? 8 : 12);
	}
}

void codegen::helper::append_eob(CodeBatch& batch, uintptr_t ptr) {
	load_address(batch, ptr, helper::address_destination);
	jump_table::jump_reroute_static(batch);
}

void codegen::helper::append_eob(CodeBatch& batch, encoding::RiscVRegister reg) {
	if (reg != helper::address_destination)
		batch += encoding::MV(helper::address_destination, reg);
	jump_table::jump_reroute_dynamic(batch);
}

void codegen::helper::sign_extend_register(codegen::CodeBatch& batch, RiscVRegister dest, RiscVRegister src, size_t byte) {
	if (byte == 4) {
		batch += encoding::ADDW(dest, src, RiscVRegister::zero);
		return;
	}

	const auto shamt = (sizeof(size_t) - byte) * 8;
	if (shamt > 0) {
		batch += encoding::SLLI(dest, src, shamt);
		batch += encoding::SRAI(dest, dest, shamt);
	}
}

std::pair<codegen::jump_table::Entry, codegen::jump_table::Entry>
codegen::helper::calculate_entries(jump_table::Entry carry, jump_table::Entry overflow, uint8_t size) {
	switch (size) {
		case 8:
			return {jump_table::Entry(static_cast<uint16_t>(carry) + 3),
					jump_table::Entry(static_cast<uint16_t>(overflow) + 3)};
		case 4:
			return {jump_table::Entry(static_cast<uint16_t>(carry) + 2),
					jump_table::Entry(static_cast<uint16_t>(overflow) + 2)};
		case 2:
			return {jump_table::Entry(static_cast<uint16_t>(carry) + 1),
					jump_table::Entry(static_cast<uint16_t>(overflow) + 1)};
		case 1:
			return {carry, overflow};

		default:
			dispatcher::Dispatcher::fault_exit("Invalid operand size");
			return {};
	}
}