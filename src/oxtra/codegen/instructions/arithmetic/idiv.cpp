#include "idiv.h"
#include "oxtra/codegen/helper.h"

void codegen::Idiv::generate(codegen::CodeBatch& batch) const {
	// currently using two of the tmps are marked as reserved for helper-functions, maybe change
	// currently employing the "risky" version of 64 bit division

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
		// "risky" version, neglect rdx, assume that numbers bigger than INT_MAX are never used
		batch += encoding::MV(dividend, rax);
	}

	// 1b. check for overflow
	/*
	if (idiv)
		if (dividend == -2^(xlen-1) && divisor == -1)
			// raise #DE (overflow)
	*/

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
		batch += encoding::MV(rax, quotient);
		batch += encoding::MV(rdx, remainder);
	}

	// if anyone wants to implement 64 bit division in riscv assembly, please consult
	// Knuth, Donald E.: The Art of Computer Programming, Volume 2: Seminumerical Algorithms, Chapter 4.3 Multiple-Precision Arithmetic

	// software implementation, using gcc feature __int128
	// need access to guest context to read the registers
	/* 128_bit_division() {
			uint64_t upper = *rdx;
			uint64_t lower = *rax;
			if (idiv) {
				__int128 dividend = (upper << 64) + lower;
				__int128 divisor_raw = *divisor; // from translate_operand
				//if (dividend == -2^(xlen-1) && divisor == -1)
				//	// raise #DE (overflow)
				int64_t quotient = dividend / divisor_raw;
				int64_t remainder = dividend % divisor_raw;
				*rax = quotient;
				*rdx = remainder;
			} else {
				unsigned __int128 dividend = (upper << 64) + lower;
				unsigned __int128 divisor_raw = *divisor;
				uint64_t quotient = dividend / divisor_raw;
				uint64_t remainder = dividend % divisor_raw;
				*rax = quotient;
				*rdx = remainder;
			}
		}
	*/
}
