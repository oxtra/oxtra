#include "oxtra/codegen/helper.h"
#include "jcc.h"

void codegen::Jcc::generate_jump(codegen::CodeBatch& batch) const {
	if (get_operand(0).get_type() == fadec::OperandType::imm) {
		helper::append_eob(batch, get_immediate());
	} else {
		translate_operand(batch, 0, 0, helper::address_destination, encoding::RiscVRegister::t0, true, false, false, false);
		helper::append_eob(batch, helper::address_destination);
	}
}

void codegen::Jcc::generate_step(codegen::CodeBatch& batch) const {
	helper::append_eob(batch, get_address() + get_size());
}

// cf == 0 && zf == 0
void codegen::Ja::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	size_t leave = batch.size();
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	// load the carry flag
	evaluate_carry(batch);

	// generate the jump to the leave
	offset = leave - batch.size();
	batch += encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// cf == 1 || zf == 1
void codegen::Jbe::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to jump to the result
	size_t finish = batch.size();
	generate_jump(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	// load the carry flag
	evaluate_carry(batch);

	// generate the jump to the out-jump
	offset = finish - batch.size();
	batch += encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	// generate the code to leave the function
	generate_step(batch);
}

// cf == 1
void codegen::Jc::generate(codegen::CodeBatch& batch) const {
	// load the carry-flag
	evaluate_carry(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// ecx == 0 / rcx == 0
void codegen::Jcxz::generate(codegen::CodeBatch& batch) const {
	// check what operand size is used
	auto reg = encoding::RiscVRegister::rcx;
	if (get_address_size() != 8) {
		batch += encoding::SLLI(encoding::RiscVRegister::t0, reg, 32);
		reg = encoding::RiscVRegister::t0;
		batch += encoding::SRLI(reg, reg, 32);
	}

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNQZ(reg, offset * 4);

	generate_jump(batch);
}

//(ZF == 0 && SF == OF)
void codegen::Jg::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	size_t leave = batch.size();
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t5);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	// load the overflow-flag
	evaluate_overflow(batch);

	// generate the jump to the leave
	offset = leave - batch.size();
	batch += encoding::BNE(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, offset * 4);

	generate_jump(batch);
}

// sf == of
void codegen::Jge::generate(codegen::CodeBatch& batch) const {
	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t3);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQ(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, offset * 4);

	generate_jump(batch);
}

// sf != of
void codegen::Jl::generate(codegen::CodeBatch& batch) const {
	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t5);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNE(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, offset * 4);

	generate_jump(batch);
}

// zf == 1 || sf != of
void codegen::Jle::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag (first, as it is easier to compute)
	evaluate_zero(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to jump to the result
	size_t finish = batch.size();
	generate_jump(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	// load the sign flag
	evaluate_sign(batch, encoding::RiscVRegister::t0);
	batch += encoding::MV(encoding::RiscVRegister::t0, encoding::RiscVRegister::t4);

	// load the overflow-flag
	evaluate_overflow(batch);

	// generate the jump to the result
	offset = finish - batch.size();
	batch += encoding::BNE(encoding::RiscVRegister::t4, encoding::RiscVRegister::t0, offset * 4);

	// generate the code to leave the jump
	generate_step(batch);
}

// cf == 0
void codegen::Jnc::generate(codegen::CodeBatch& batch) const {
	// load the carry-flag
	evaluate_carry(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
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
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// pf == 0
void codegen::Jnp::generate(codegen::CodeBatch& batch) const {
	// load the parity-flag
	evaluate_parity(batch, encoding::RiscVRegister::t0);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// sf == 0
void codegen::Jns::generate(codegen::CodeBatch& batch) const {
	// load the sign-flag
	evaluate_sign(batch, encoding::RiscVRegister::t0);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// zf == 0
void codegen::Jnz::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag
	evaluate_zero(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BEQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// of == 1
void codegen::Jo::generate(codegen::CodeBatch& batch) const {
	// load the overflow-flag
	evaluate_overflow(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// pf == 1
void codegen::Jp::generate(codegen::CodeBatch& batch) const {
	// load the parity-flag
	evaluate_parity(batch, encoding::RiscVRegister::t0);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// sf == 1
void codegen::Js::generate(codegen::CodeBatch& batch) const {
	// load the sign-flag
	evaluate_sign(batch, encoding::RiscVRegister::t0);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}

// zf == 1
void codegen::Jz::generate(codegen::CodeBatch& batch) const {
	// load the zero-flag
	evaluate_zero(batch);

	// append a dummy-branch
	size_t index = batch.add(encoding::NOP());

	// generate the code to leave the jump
	generate_step(batch);

	// compute the offset and generate the jump
	size_t offset = batch.size() - index;
	batch[index] = encoding::BNQZ(encoding::RiscVRegister::t4, offset * 4);

	generate_jump(batch);
}