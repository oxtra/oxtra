#include "oxtra/codegen/helper.h"
#include "oxtra/codegen/encoding/encoding.h"
#include "instruction.h"
#include <spdlog/spdlog.h>

using namespace encoding;


void codegen::helper::move_to_register(CodeBatch& batch, RiscVRegister dest, RiscVRegister src, RegisterAccess access,
									   RiscVRegister temp, bool cleared) {
	switch (access) {
		case RegisterAccess::QWORD:
			batch += encoding::ADD(dest, src, RiscVRegister::zero);
			return;
		case RegisterAccess::DWORD:
			// copy the source-register and clear the upper bits by shifting
			batch += encoding::SLLI(dest, src, 32);
			batch += encoding::SRLI(dest, dest, 32);
			return;
		case RegisterAccess::WORD:
			// check if the upper source-register has to be cleared
			if (!cleared) {
				batch += encoding::XOR(temp, src, dest);
				batch += encoding::SLLI(temp, temp, 48);
				batch += encoding::SRLI(temp, temp, 48);
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

			// clear the upper 48 bits of the temp register and keep the lower 8 cleared
			batch += encoding::SLLI(temp, temp, 56);
			batch += encoding::SRLI(temp, temp, 48);

			// xor the temporary register to the destination
			batch += encoding::XOR(dest, temp, dest);
			return;
	}
}

static void load_12bit_immediate(codegen::CodeBatch& batch, uint16_t imm, RiscVRegister dest) {
	batch += encoding::ADDI(dest, RiscVRegister::zero, static_cast<uint16_t>(imm) & 0x0FFFu);
}

static void load_32bit_immediate(codegen::CodeBatch& batch, uint32_t imm, RiscVRegister dest, bool optimize) {
	auto upper_immediate = static_cast<uint32_t>(imm >> 12u);
	const auto lower_immediate = static_cast<uint16_t>(imm & 0x0FFFu);

	// adding the lower bits is sign extended, so if the lower bits are signed we have to increase the upper immediate
	if (imm & 0x800u)
		upper_immediate++;

	batch += encoding::LUI(dest, upper_immediate);

	if (!optimize || lower_immediate != 0x0000) {
		batch += encoding::ADDI(dest, dest, lower_immediate);
	}
}

static void load_64bit_immediate(codegen::CodeBatch& batch, uint64_t imm, RiscVRegister dest, bool optimize) {
	const uint32_t high_bits = (imm >> 32u) & 0xFFFFFFFF;
	const uint32_t low_bits = imm & 0xFFFFFFFF;
	constexpr size_t immediate_count = 4;
	uint32_t immediates[immediate_count] = {low_bits & 0x000000FFu, (low_bits & 0x000FFF00u) >> 8u,
											(low_bits & 0xFFF00000u) >> 20u, high_bits};

	for (size_t i = immediate_count - 2; i >= 1; i--) {
		if (immediates[i] & 0x800u)
			immediates[i + 1]++;
	}
	// load upper 32bit into destination
	load_32bit_immediate(batch, immediates[immediate_count - 1], dest, optimize);

	for (int8_t i = immediate_count - 2; i >= 0; i--) {
		// add the next 12bit (8 bit for the last one)
		batch += encoding::SLLI(dest, dest, (i == 0) ? 8 : 12);
		if (!optimize || immediates[i] != 0) {
			batch += encoding::ADDI(dest, dest, immediates[i]);
		}
	}
}

void codegen::helper::load_immediate(CodeBatch& batch, uintptr_t imm, encoding::RiscVRegister dest) {
	/* 00000000 00000000 00001111 11111111 22222222 22223333 33333333 44444444 */

	// < 12-bit
	if (imm < 0x800) {
		load_12bit_immediate(batch, imm, dest);
	}

		// < 32-bit
	else if (imm < 0x80000000) {
		load_32bit_immediate(batch, imm, dest, true);
	}

		// >= 32 bit
	else {
		load_64bit_immediate(batch, imm, dest, true);
	}
}

void codegen::helper::load_address(codegen::CodeBatch& batch, uintptr_t ptr, encoding::RiscVRegister dest) {
	load_64bit_immediate(batch, ptr, dest, false);
}

void codegen::helper::append_eob(CodeBatch& batch, uintptr_t ptr) {
	load_address(batch, ptr, helper::address_destination);
	jump_table::jump_reroute_static(batch);
}

void codegen::helper::append_eob(CodeBatch& batch, encoding::RiscVRegister reg) {
	if (reg != helper::address_destination) {
		batch += encoding::MV(helper::address_destination, reg);
	}

	jump_table::jump_reroute_dynamic(batch);
}

void codegen::helper::sign_extend_register(codegen::CodeBatch& batch, RiscVRegister dest, RiscVRegister src, size_t byte) {
	//TODO: ADDIW for better performance
	const auto shamt = (sizeof(size_t) - byte) * 8;
	if (shamt > 0) {
		batch += encoding::SLLI(dest, src, shamt);
		batch += encoding::SRAI(dest, dest, shamt);
	}
}