#include "idiv.h"
#include "oxtra/codegen/helper.h"
#include "oxtra/dispatcher/dispatcher.h"

void codegen::Idiv::generate(codegen::CodeBatch& batch) const {
	// currently using the "risky" version of 128 bit division

	const auto idiv = (get_type() == fadec::InstructionType::IDIV) ? true : false;
	const auto op_size = get_operand(0).get_size();
	constexpr auto rax = helper::map_reg(fadec::Register::rax);
	constexpr auto rdx = helper::map_reg(fadec::Register::rdx);
	constexpr auto tmp = encoding::RiscVRegister::t4;

	// dividend / divisor = quotient ; remainder
	constexpr auto quotient = encoding::RiscVRegister::t3;
	constexpr auto remainder = encoding::RiscVRegister::t2;
	constexpr auto dividend = encoding::RiscVRegister::t1;
	constexpr auto divisor = encoding::RiscVRegister::t0;
	translate_operand(batch, 0, divisor, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2);

	// if (divisor == 0) raise #DE (divide by zero)
	batch += BNQZ(divisor, 10*4);
	helper::load_address(batch, reinterpret_cast<uintptr_t>(&dispatcher::Dispatcher::fault_exit), encoding::RiscVRegister::t4);
	jump_table_entry(batch, jump_table::Entry::debug_callback);

#if 0
	if (op_size == 8) {
		if (idiv)
			batch += ADDI(encoding::RiscVRegister::t1, encoding::RiscVRegister::zero, 1);
		else
			batch += MV(encoding::RiscVRegister::t1, encoding::RiscVRegister::zero);
		batch += ADDI(encoding::RiscVRegister::t2, encoding::RiscVRegister::zero, 4);
		helper::load_address(batch, reinterpret_cast<uintptr_t>(&helper::idiv_128_bits), encoding::RiscVRegister::t4);
		jump_table_entry(batch, jump_table::Entry::debug_callback);
		return;
	}
#endif

	// 1. build dividend
	if (op_size == 1) {
		// dividend = ax
		batch += encoding::MV(dividend, encoding::RiscVRegister::zero);
		move_to_register( batch, dividend, rax, helper::RegisterAccess::WORD, tmp);
	} else if (op_size == 2 || op_size == 4) {
		// dividend = dx:ax bzw. edx:eax
		const auto reg_access = (op_size < 3) ? helper::RegisterAccess::WORD : helper::RegisterAccess::DWORD;
		const auto shamt = (op_size < 3) ? 16 : 32;
		batch += encoding::MV(dividend, encoding::RiscVRegister::zero);
		move_to_register(batch, dividend, rdx, reg_access, tmp);
		batch += encoding::SLLI(dividend, dividend, shamt);
		move_to_register(batch, encoding::RiscVRegister::t2, rax, reg_access, tmp);
		batch += encoding::ADD(dividend, dividend, encoding::RiscVRegister::t2);
	} else {
		// dividend = rax, assumes values larger than INT_MAX are never used
		batch += encoding::MV(dividend, rax);
	}

	// 1b. sign-extension
	if (idiv && op_size < 4) {
		const auto shamt = (op_size < 2) ? 48 : 32;
		batch += encoding::SLLI(dividend, dividend, shamt);
		batch += encoding::SRAI(dividend, dividend, shamt);
	}

	// 1c. check for overflow
	/*
	if (idiv)
		if (dividend == -2^(xlen-1) && divisor == -1)
			// raise #DE (overflow)
	*/
	helper::load_immediate(batch, -1, tmp);
	batch += BNE(tmp, divisor, 19*4);
	if (idiv) { // using load_address to guarantee 8 instructions
		if (op_size == 1) {
			helper::load_address(batch, -(2<<6), tmp);
		} else if (op_size == 2) {
			helper::load_address(batch, -(2<<14), tmp);
		} else if (op_size == 4) {
			helper::load_address(batch, -(2<<30), tmp);
		} else {
			helper::load_address(batch, -(static_cast<uintptr_t>(2)<<62), tmp);
		}
	}
	batch += BNE(tmp, dividend, 10*4);
	helper::load_address(batch, reinterpret_cast<uintptr_t>(&dispatcher::Dispatcher::fault_exit), encoding::RiscVRegister::t4);
	jump_table_entry(batch, jump_table::Entry::debug_callback);

	// 2. do the division
	if (idiv) {
		batch += encoding::DIV(quotient, dividend, divisor);
		batch += encoding::REM(remainder, dividend, divisor);
	} else {
		batch += encoding::DIVU(quotient, dividend, divisor);
		batch += encoding::REMU(remainder, dividend, divisor);
	}

	// 3. store the result
	if (op_size == 1) {
		move_to_register(batch, rax, quotient, helper::RegisterAccess::LBYTE, tmp);
		move_to_register(batch, rax, remainder, helper::RegisterAccess::HBYTE, tmp);
	} else if (op_size == 2 || op_size == 4) {
		helper::RegisterAccess reg_access = (op_size < 3) ? helper::RegisterAccess::WORD : helper::RegisterAccess::DWORD;
		move_to_register(batch, rax, quotient, reg_access, tmp);
		move_to_register(batch, rdx, remainder, reg_access, tmp);
	} else {
		batch += MV(rax, quotient);
		batch += MV(rdx, remainder);
	}
}
