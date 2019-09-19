#include "setcc.h"
#include "oxtra/codegen/helper.h"

void codegen::Setcc::set_value(CodeBatch& batch, encoding::RiscVRegister reg) const {
	const auto& dst = get_operand(0);

	// if the operand is a register then just write the value into the register
	if (dst.get_type() == fadec::OperandType::reg) {
		const auto dst_reg = helper::map_reg(dst.get_register(), dst.get_register_type());
		helper::move_to_register(batch, dst_reg, reg, dst.get_register_type() == fadec::RegisterType::gph ? 0 : 1,
								 encoding::RiscVRegister::t0, true);
	}

		// if the operand is memory then store it in a temp register and store that to memore
	else /*if (dst.get_type() == fadec::OperandType::mem)*/ {
		write_to_memory(batch, 0, reg, encoding::RiscVRegister::t0, encoding::RiscVRegister::t1,
						encoding::RiscVRegister::zero);
	}
}

// (cf || zf) == 0
void codegen::Seta::generate(codegen::CodeBatch& batch) const {
	evaluate_zero(batch);

	// move the result in another register so that we don't override it
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	evaluate_carry(batch);

	batch += encoding::OR(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	batch += encoding::SEQZ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0);

	set_value(batch, encoding::RiscVRegister::t0);
}

// (cf || zf) == 1
void codegen::Setbe::generate(CodeBatch& batch) const {
	evaluate_zero(batch);

	// move the result in another register so that we don't override it
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	evaluate_carry(batch);

	batch += encoding::OR(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	set_value(batch, encoding::RiscVRegister::t0);
}

// cf == 1
void codegen::Setc::generate(CodeBatch& batch) const {
	evaluate_carry(batch);
	set_value(batch, encoding::RiscVRegister::t4);
}

// ((sf ^ of) || zf) == 0
void codegen::Setg::generate(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t0);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	evaluate_overflow(batch);
	batch += encoding::XOR(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	evaluate_zero(batch);
	batch += encoding::OR(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	batch += encoding::SEQZ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0);
	set_value(batch, encoding::RiscVRegister::t0);
}

// (sf ^ of) == 0
void codegen::Setge::generate(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t0);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	evaluate_overflow(batch);
	batch += encoding::XOR(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	batch += encoding::SEQZ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0);
	set_value(batch, encoding::RiscVRegister::t0);
}

// (sf ^ of) == 1
void codegen::Setl::generate(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t0);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	evaluate_overflow(batch);
	batch += encoding::XOR(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	set_value(batch, encoding::RiscVRegister::t0);
}

// ((sf ^ of) || zf) == 1
void codegen::Setle::generate(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t0);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	evaluate_overflow(batch);
	batch += encoding::XOR(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	evaluate_zero(batch);
	batch += encoding::OR(encoding::RiscVRegister::t0, encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	set_value(batch, encoding::RiscVRegister::t0);
}

// cf == 0
void codegen::Setnc::generate(CodeBatch& batch) const {
	evaluate_carry(batch);
	batch += encoding::SEQZ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	set_value(batch, encoding::RiscVRegister::t0);
}

// of == 0
void codegen::Setno::generate(CodeBatch& batch) const {
	evaluate_overflow(batch);
	batch += encoding::SEQZ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	set_value(batch, encoding::RiscVRegister::t0);
}

// pf == 0
void codegen::Setnp::generate(CodeBatch& batch) const {
	evaluate_parity(batch, encoding::RiscVRegister::t0);
	batch += encoding::SEQZ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	set_value(batch, encoding::RiscVRegister::t0);
}

// sf == 0
void codegen::Setns::generate(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t0);
	batch += encoding::SEQZ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	set_value(batch, encoding::RiscVRegister::t0);
}

// zf == 0
void codegen::Setnz::generate(CodeBatch& batch) const {
	evaluate_zero(batch);
	batch += encoding::SEQZ(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);
	set_value(batch, encoding::RiscVRegister::t0);
}

// of == 1
void codegen::Seto::generate(CodeBatch& batch) const {
	evaluate_overflow(batch);
	set_value(batch, encoding::RiscVRegister::t4);
}

// pf == 1
void codegen::Setp::generate(CodeBatch& batch) const {
	evaluate_parity(batch, encoding::RiscVRegister::t0);
	set_value(batch, encoding::RiscVRegister::t4);
}

// sf == 1
void codegen::Sets::generate(CodeBatch& batch) const {
	evaluate_sign(batch, encoding::RiscVRegister::t0);
	set_value(batch, encoding::RiscVRegister::t4);
}

// zf == 1
void codegen::Setz::generate(CodeBatch& batch) const {
	evaluate_zero(batch);
	set_value(batch, encoding::RiscVRegister::t4);
}