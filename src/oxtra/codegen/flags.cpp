#include "flags.h"
#include "encoding/encoding.h"

using namespace encoding;

/*
 * TODO move update functionality into evaluate functions and implement update.
 */

void codegen::flags::update_zero_flag(encoding::RiscVRegister reg, uint8_t reg_size,
									  utils::riscv_instruction_t* riscv, size_t& count) {
}

void codegen::flags::evaluate_zero_flag(encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {
#if 0
	// shift the upper bits, in order to clear them
	if (reg_size != 8) {
		riscv[count++] = encoding::SLLI(temp, reg, 64 - 8 * reg_size);
		reg = temp;
		riscv[count++] = encoding::SRLI(reg, reg, 64 - 8 * reg_size);
	}

	/* The zero flag is fairly simple to test.
	 * We just check if the value in the register is zero,
	 * and clear or set the flag-bit accordingly. */
	riscv[count++] = encoding::BNQZ(reg, 12u);
	riscv[count++] = encoding::ORI(flag_register, flag_register, 0x40);
	riscv[count++] = encoding::J(8u);
	riscv[count++] = encoding::ANDI(flag_register, flag_register, -0x41);
#endif
}

void codegen::flags::update_sign_flag(encoding::RiscVRegister reg, uint8_t reg_size, encoding::RiscVRegister temp,
									  utils::riscv_instruction_t* riscv, size_t& count) {
#if 0
	// extract the lowest bit
	riscv[count++] = encoding::SRLI(temp, reg, reg_size * 8 - 1);
	riscv[count++] = encoding::ANDI(temp, temp, 0x01);

	/* In order to test for the sign-bit,
	 * we just check if the lowest bit is set,
	 * after the we have isolated it. */
	riscv[count++] = encoding::BEQZ(temp, 12u);
	riscv[count++] = encoding::ORI(flag_register, flag_register, 0x80);
	riscv[count++] = encoding::J(8u);
	riscv[count++] = encoding::ANDI(flag_register, flag_register, -0x81);
#endif
}

void codegen::flags::evaluate_sign_flag(encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {

}

void codegen::flags::update_parity_flag(encoding::RiscVRegister reg, uint8_t reg_size,
										utils::riscv_instruction_t* riscv, size_t& count) {
#if 0
	// (reg >> 32) ^ reg = temp
	// (a >> 16) ^ a
	// (a >> 8) ^ a
	// (a >> 4) ^ a
	// (a >> 2) ^ a
	// (a >> 1) ^ a

	// compute the initial index
	reg_size = (reg_size * 8) / 2;

	// load the initial shift
	riscv[count++] = encoding::SRLI(temp_b, reg, reg_size);
	riscv[count++] = encoding::XOR(temp_a, reg, temp_b);
	reg_size /= 2;

	// enter the loop to add the missing instructions
	while(reg_size > 1) {
		riscv[count++] = encoding::SRLI(temp_b, temp_a, reg_size);
		riscv[count++] = encoding::XOR(temp_a, temp_a, temp_b);
		reg_size /= 2;
	}
	riscv[count++] = encoding::ANDI(temp_a, temp_a, 0x01);

	// set the flag
	riscv[count++] = encoding::BNQZ(temp_a, 12u);
	riscv[count++] = encoding::ORI(flag_register, flag_register, 0x10);
	riscv[count++] = encoding::J(8u);
	riscv[count++] = encoding::ANDI(flag_register, flag_register, -0x11);
#endif
}

void codegen::flags::evaluate_parity_flag(encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {

}

void codegen::flags::update_carry_flag(encoding::RiscVRegister reg_a, encoding::RiscVRegister reg_b, uint8_t reg_size,
									   FlagOperation operation, utils::riscv_instruction_t* riscv, size_t& count) {

}

void codegen::flags::evaluate_carry_flag(encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {

}

void codegen::flags::update_overflow_flag(encoding::RiscVRegister reg_a, encoding::RiscVRegister reg_b, uint8_t reg_size,
										  FlagOperation operation, utils::riscv_instruction_t* riscv, size_t& count) {

}

void codegen::flags::evaluate_overflow_flag(encoding::RiscVRegister reg, utils::riscv_instruction_t* riscv, size_t& count) {

}