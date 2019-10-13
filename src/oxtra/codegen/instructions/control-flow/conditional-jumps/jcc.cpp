#include "oxtra/codegen/helper.h"
#include "jcc.h"

void codegen::Jcc::generate_jump(codegen::CodeBatch& batch) const {
	if (get_operand(0).get_type() == fadec::OperandType::imm) {
		helper::append_eob(batch, get_immediate());
	} else {
		translate_operand(batch, get_operand(0), 0, helper::address_destination, encoding::RiscVRegister::t0, true, false, false, false);
		helper::append_eob(batch, helper::address_destination);
	}
}

// cf == 0 && zf == 0
void codegen::Ja::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch, encoding::RiscVRegister::t4);

	// append a dummy-branch
	size_t dummy0 = batch.add(encoding::NOP());

	// load the carry flag
	evaluate_carry(batch);

	// append the dummy-branch
	size_t dummy1 = batch.add(encoding::NOP());

	// generate the jump
	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(dummy0, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(dummy0, batch.size()) * 4));

	// generate the jump to the leave
	batch.insert(dummy1, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(dummy1, batch.size()) * 4));
}

// cf == 1 || zf == 1
void codegen::Jbe::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch, encoding::RiscVRegister::t4);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to jump to the result
	size_t finish = batch.size();
	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BEQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));

	// load the carry flag
	evaluate_carry(batch);

	// generate the jump to the out-jump
	batch.add(encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(batch.size(), finish) * 4));
}

// cf == 1
void codegen::Jc::generate(codegen::CodeBatch& batch) const {
	// load the carry-flag
	evaluate_carry(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to jump
	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BEQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// ecx == 0 / rcx == 0
void codegen::Jcxz::generate(codegen::CodeBatch& batch) const {
	// check what operand size is used
	auto reg = helper::map_reg(fadec::Register::rcx);
	if (get_address_size() != 8) {
		batch += encoding::SLLI(encoding::RiscVRegister::t0, reg, 32);
		reg = encoding::RiscVRegister::t0;
		batch += encoding::SRLI(reg, reg, 32);
	}

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to jump
	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// zf == 0 && sf == of
void codegen::Jg::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch, encoding::RiscVRegister::t4);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::t5);

	// load the overflow-flag
	evaluate_overflow(batch);

	// generate the code to leave the jump
	size_t leave = batch.size();
	generate_jump(batch);

	// if sf != of then don't go to the jump
	batch.insert(leave, encoding::BNE(encoding::RiscVRegister::t4, encoding::RiscVRegister::t5, batch.offset(leave, batch.size())));

	// compute the offset and generate the jump
	batch.insert(index, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// sf == of
void codegen::Jge::generate(codegen::CodeBatch& batch) const {
	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::t5);

	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BNE(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, batch.offset(index, batch.size()) * 4));
}

// sf != of
void codegen::Jl::generate(codegen::CodeBatch& batch) const {
	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::t5);

	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BEQ(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, batch.offset(index, batch.size()) * 4));
}

// zf == 1 || sf != of
void codegen::Jle::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch, encoding::RiscVRegister::t4);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to jump to the result
	size_t finish = batch.size();
	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BEQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));

	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t0, encoding::RiscVRegister::t5);

	// load the overflow-flag
	evaluate_overflow(batch);

	// generate the jump to the result
	batch.add(encoding::BNE(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, batch.offset(batch.size(), finish) * 4));
}

// cf == 0
void codegen::Jnc::generate(codegen::CodeBatch& batch) const {
	// load the carry-flag
	evaluate_carry(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// of == 0
void codegen::Jno::generate(codegen::CodeBatch& batch) const {
	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// pf == 0
void codegen::Jnp::generate(codegen::CodeBatch& batch) const {
	// load the parity-flag
	evaluate_parity(batch, encoding::RiscVRegister::t4, encoding::RiscVRegister::t5);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// sf == 0
void codegen::Jns::generate(codegen::CodeBatch& batch) const {
	// load the sign-flag
	evaluate_sign(batch, encoding::RiscVRegister::t4, encoding::RiscVRegister::t0);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// zf == 0
void codegen::Jnz::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag
	evaluate_zero(batch, encoding::RiscVRegister::t4);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BNQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// of == 1
void codegen::Jo::generate(codegen::CodeBatch& batch) const {
	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BEQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// pf == 1
void codegen::Jp::generate(codegen::CodeBatch& batch) const {
	// load the parity-flag
	evaluate_parity(batch, encoding::RiscVRegister::t4, encoding::RiscVRegister::t5);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BEQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// sf == 1
void codegen::Js::generate(codegen::CodeBatch& batch) const {
	// load the sign-flag
	evaluate_sign(batch, encoding::RiscVRegister::t4, encoding::RiscVRegister::t0);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BEQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}

// zf == 1
void codegen::Jz::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag
	evaluate_zero(batch, encoding::RiscVRegister::t4);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	generate_jump(batch);

	// compute the offset and generate the jump
	batch.insert(index, encoding::BEQZ(encoding::RiscVRegister::t4, batch.offset(index, batch.size()) * 4));
}