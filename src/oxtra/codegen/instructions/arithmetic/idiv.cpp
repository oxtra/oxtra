#include "idiv.h"
#include "oxtra/codegen/helper.h"
#include "oxtra/dispatcher/dispatcher.h"

void codegen::Idiv::generate(codegen::CodeBatch& batch) const {
	// currently using the "risky" version of 128 bit division

	const auto idiv = get_type() == fadec::InstructionType::IDIV;
	const auto op_size = get_operand(0).get_size();
	constexpr auto rax = helper::map_reg(fadec::Register::rax);
	constexpr auto rdx = helper::map_reg(fadec::Register::rdx);
	constexpr auto tmp = encoding::RiscVRegister::t4;

	// dividend / divisor = quotient ; remainder
	constexpr auto quotient = encoding::RiscVRegister::t3;
	constexpr auto remainder = encoding::RiscVRegister::t2;
	constexpr auto dividend = encoding::RiscVRegister::t1;
	constexpr auto divisor = encoding::RiscVRegister::t0;
	translate_operand(batch, 0, 0, divisor, tmp, true, true, idiv, false);

	// if (divisor == 0) raise #DE (divide by zero)
	auto branch = batch.add(encoding::NOP());
	call_high_level(batch, &codegen::Idiv::division_exception);
	batch.insert(branch, encoding::BNQZ(divisor, (batch.size() - branch)*4));

	// 1. build dividend
	if (op_size == 1) {
		// dividend = ax
		helper::load_from_register(batch, rax, helper::RegisterAccess::WORD, dividend, true, true, idiv);
	} else if (op_size == 2 || op_size == 4) {
		// dividend = dx:ax bzw. edx:eax
		const auto reg_access = (op_size < 3) ? helper::RegisterAccess::WORD : helper::RegisterAccess::DWORD;
		const auto shamt = (op_size < 3) ? 16 : 32;
		helper::load_from_register(batch, rdx, reg_access, dividend, true, true, idiv);
		batch += encoding::SLLI(dividend, dividend, shamt);
		helper::load_from_register(batch, rax, reg_access, tmp, true, false, false);
		batch += encoding::ADD(dividend, dividend, tmp);
	} else {
		// dividend = rax, assumes values larger than INT_MAX are never used
		batch += encoding::MV(dividend, rax);
	}

	// 1c. check for overflow
	/*
	if (idiv)
		if (dividend == int_min && divisor == -1)
			// raise #DE (overflow)
	*/
	if (idiv) { // using load_address to guarantee 8 instructions
		helper::load_immediate(batch, -1, tmp);
		auto branch_a = batch.add(encoding::NOP());
		if (op_size == 1) {
			helper::load_immediate(batch, -32768 , tmp);
		} else if (op_size == 2) {
			helper::load_immediate(batch, -2147483648, tmp);
		} else if (op_size == 4) {
			helper::load_immediate(batch, -9223372036854775808, tmp);
		} else {
			helper::load_immediate(batch, -9223372036854775808, tmp);
		}
		auto branch_b = batch.add(encoding::NOP());
		call_high_level(batch, &codegen::Idiv::division_exception);
		batch.insert(branch_a, encoding::BNE(tmp, divisor, (batch.size() - branch_a)*4));
		batch.insert(branch_b, encoding::BNE(tmp, dividend, (batch.size() - branch_b)*4));
	}

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
		helper::move_to_register(batch, rax, quotient, helper::RegisterAccess::LBYTE, tmp, false);
		helper::move_to_register(batch, rax, remainder, helper::RegisterAccess::HBYTE, tmp, false);
	} else if (op_size == 2 || op_size == 4) {
		auto reg_access = (op_size < 3) ? helper::RegisterAccess::WORD : helper::RegisterAccess::DWORD;
		helper::move_to_register(batch, rax, quotient, reg_access, tmp, false);
		helper::move_to_register(batch, rdx, remainder, reg_access, tmp, false);
	} else {
		batch += MV(rax, quotient);
		batch += MV(rdx, remainder);
	}
}

unsigned long codegen::Idiv::division_exception(dispatcher::ExecutionContext* context) {
	unused_parameter(context);
	dispatcher::Dispatcher::fault_exit("Divide exception");
	return 0;
}
