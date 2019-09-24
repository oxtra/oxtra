#include "shift.h"
#include "oxtra/codegen/helper.h"

void codegen::ShiftCl::generate(codegen::CodeBatch& batch) const {
	const auto& dst = get_operand(0);
	const auto operand_size = dst.get_size();
	if (dst.get_type() == fadec::OperandType::reg) {
		const auto dst_reg = helper::map_reg(dst.get_register(), dst.get_register_type());

		if (operand_size == 8) {
			execute_operation(batch, dst_reg, dst_reg);
		} else if (operand_size == 4) {
			execute_operation(batch, dst_reg, dst_reg);
			batch += encoding::SLLI(dst_reg, dst_reg, 32);
			batch += encoding::SRLI(dst_reg, dst_reg, 32);
		} else {
			constexpr auto temp = encoding::RiscVRegister::t0;
			if (dst.get_register_type() == fadec::RegisterType::gph) {
				batch += encoding::SRLI(temp, dst_reg, 8);
				execute_operation(batch, temp, temp);
				helper::move_to_register(batch, dst_reg, temp, 0, encoding::RiscVRegister::t1, false);
			} else {
				execute_operation(batch, temp, dst_reg);
				helper::move_to_register(batch, dst_reg, temp, operand_size, encoding::RiscVRegister::t1, false);
			}
		}
	} else /*if (dst.get_type() == fadec::OperandType::mem)*/ {
		constexpr auto temp = encoding::RiscVRegister::t0;
		const auto address = read_from_memory(batch, dst, temp, encoding::RiscVRegister::t1, false);
		execute_operation(batch, temp, temp);
		write_to_memory(batch, dst, temp, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2, address);
	}
}

void codegen::Sar::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRA(dst, dst, src);
	update_overflow_unsupported(batch, "SAR", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SAR", encoding::RiscVRegister::t4);
}

void codegen::Sar::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SRAI(dst, dst, imm);
	update_overflow_unsupported(batch, "SAR - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SAR - Imm", encoding::RiscVRegister::t4);
}

void codegen::SarCl::execute_operation(codegen::CodeBatch& batch,
									   encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	if (get_operand(0).get_size() == 8) {
		batch += encoding::SRA(dst, src, helper::map_reg(fadec::Register::cl));
	} else {
		batch += encoding::SRAW(dst, src, helper::map_reg(fadec::Register::cl));
	}

	update_overflow_unsupported(batch, "SAR - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SAR - Imm", encoding::RiscVRegister::t4);
}

void codegen::Shl::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SLL(dst, dst, src);
	update_overflow_unsupported(batch, "SHL", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL", encoding::RiscVRegister::t4);
}

void codegen::Shl::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SLLI(dst, dst, imm);
	update_overflow_unsupported(batch, "SHL - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL - Imm", encoding::RiscVRegister::t4);
}

void codegen::ShlCl::execute_operation(codegen::CodeBatch& batch,
									   encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	if (get_operand(0).get_size() == 8) {
		batch += encoding::SLL(dst, src, helper::map_reg(fadec::Register::cl));
	} else {
		batch += encoding::SLLW(dst, src, helper::map_reg(fadec::Register::cl));
	}

	update_overflow_unsupported(batch, "SHL - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHL - Imm", encoding::RiscVRegister::t4);
}

void codegen::Shr::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	batch += encoding::SRL(dst, dst, src);
	update_overflow_unsupported(batch, "SHR", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHR", encoding::RiscVRegister::t4);
}

void codegen::Shr::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, intptr_t imm) const {
	batch += encoding::SRLI(dst, dst, imm);
	update_overflow_unsupported(batch, "SHR - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHR - Imm", encoding::RiscVRegister::t4);
}

void codegen::ShrCl::execute_operation(codegen::CodeBatch& batch,
									   encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	if (get_operand(0).get_size() == 8) {
		batch += encoding::SRL(dst, src, helper::map_reg(fadec::Register::cl));
	} else {
		batch += encoding::SRLW(dst, src, helper::map_reg(fadec::Register::cl));
	}

	update_overflow_unsupported(batch, "SHR - Imm", encoding::RiscVRegister::t4);
	update_carry_unsupported(batch, "SHR - Imm", encoding::RiscVRegister::t4);
}