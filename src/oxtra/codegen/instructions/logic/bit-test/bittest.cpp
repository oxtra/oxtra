#include "bittest.h"
#include "oxtra/codegen/helper.h"
#include "oxtra/dispatcher/dispatcher.h"

void codegen::BitTest::generate(codegen::CodeBatch& batch) const {
	const auto& bit_base = get_operand(0);
	const auto& bit_off = get_operand(1);

	const auto operand_size = bit_base.get_size();

	// the register that contains the bit base value
	auto bit_base_reg = encoding::RiscVRegister::t0;

	// this register contains the address that is passed to write_to_memory
	auto address = encoding::RiscVRegister::zero;

	// get the bit_base_reg
	if (bit_base.get_type() == fadec::OperandType::reg) {
		bit_base_reg = helper::map_reg(bit_base.get_register());
	} else /*if (bit_base.get_type() == fadec::OperandType::mem)*/ {
		address = read_from_memory(batch, 0, bit_base_reg, encoding::RiscVRegister::t3, false);
	}

	if (bit_off.get_type() == fadec::OperandType::imm) {
		// we should just be able to helper::load_immediate here, but i don't trust it with being this optimized
		const auto shift_amount = get_immediate() % (operand_size * 8);
		if (shift_amount < 11) {
			batch += encoding::ADDI(mask_reg, encoding::RiscVRegister::zero, 1u << shift_amount);
		}
		else if (shift_amount > 11 && shift_amount < 31) {
			batch += encoding::LUI(mask_reg, 1u << (shift_amount - 12u));
		}
		else {
			batch += encoding::ADDI(mask_reg, encoding::RiscVRegister::zero, 1);
			batch += encoding::SLLI(mask_reg, mask_reg, shift_amount);
		}

	} else /*if (bit_off.get_type() == fadec::OperandType::reg)*/ {
		// write a 1 into the mask. it will be shifted to produce the actual mask
		batch += encoding::ADDI(mask_reg, encoding::RiscVRegister::zero, 1);

		const auto bit_off_reg = helper::map_reg(bit_off.get_register());
		switch (operand_size) {
			case 8:
				encoding::SLL(mask_reg, mask_reg, bit_off_reg);
				break;
			case 4:
				encoding::SLLW(mask_reg, mask_reg, bit_off_reg);
				break;
			case 2:
				// bit_base % 16
				encoding::ANDI(encoding::RiscVRegister::t2, bit_off_reg, 0xf);
				encoding::SLL(mask_reg, mask_reg, bit_off_reg);
				break;
		}
	}

	//
	batch += encoding::AND(bit_value_reg, bit_base_reg, mask_reg);
	batch += encoding::SNEZ(bit_value_reg, bit_value_reg);
	batch += encoding::SLLI(bit_value_reg, bit_value_reg, 2);
	batch += encoding::ADDI(bit_value_reg, bit_value_reg, static_cast<uint16_t>(jump_table::Entry::carry_clear) * 4);

	update_carry(batch, bit_value_reg);

	manipulate_bit(batch, bit_base_reg);

	if (bit_base.get_type() == fadec::OperandType::mem) {
		write_to_memory(batch, 0, bit_base_reg, encoding::RiscVRegister::t1, encoding::RiscVRegister::t2, address);
	}
}

void codegen::Bt::generate(codegen::CodeBatch& batch) const {
	const auto& bit_base = get_operand(0);
	const auto& bit_off = get_operand(1);

	const auto operand_size = bit_base.get_size();

	auto bit_base_reg = encoding::RiscVRegister::t0;
	if (bit_base.get_type() == fadec::OperandType::reg) {
		bit_base_reg = helper::map_reg(bit_base.get_register());
	} else /*if (bit_base.get_type() == fadec::OperandType::mem)*/ {
		read_from_memory(batch, 0, bit_base_reg, encoding::RiscVRegister::t1, false);
	}

	constexpr auto bit_value_reg = encoding::RiscVRegister::t1;
	if (bit_off.get_type() == fadec::OperandType::imm) {
		batch += encoding::SRLI(bit_value_reg, bit_base_reg, get_immediate() % (operand_size * 8));

	} else /*if (bit_off.get_type() == fadec::OperandType::reg)*/ {
		const auto bit_off_reg = helper::map_reg(bit_off.get_register());
		switch (operand_size) {
			case 8:
				encoding::SRL(bit_value_reg, bit_base_reg, bit_off_reg);
				break;
			case 4:
				encoding::SRLW(bit_value_reg, bit_base_reg, bit_off_reg);
				break;
			case 2:
				// bit_base % 16
				encoding::ANDI(encoding::RiscVRegister::t2, bit_off_reg, 0xf);
				encoding::SRL(bit_value_reg, bit_base_reg, bit_off_reg);
				break;
		}
	}

	batch += encoding::ANDI(bit_value_reg, bit_value_reg, 1);
	batch += encoding::SLLI(bit_value_reg, bit_value_reg, 2);
	batch += encoding::ADDI(bit_value_reg, bit_value_reg, static_cast<uint16_t>(jump_table::Entry::carry_clear) * 4);

	update_carry(batch, bit_value_reg);
}

void codegen::Btc::manipulate_bit(codegen::CodeBatch& batch, encoding::RiscVRegister bit_base) const {
	batch += encoding::XOR(bit_base, bit_base, mask_reg);
}

void codegen::Btr::manipulate_bit(codegen::CodeBatch& batch, encoding::RiscVRegister bit_base) const {
	batch += encoding::NOT(mask_reg, mask_reg);
	batch += encoding::AND(bit_base, bit_base, mask_reg);
}

void codegen::Bts::manipulate_bit(codegen::CodeBatch& batch, encoding::RiscVRegister bit_base) const {
	batch += encoding::OR(bit_base, bit_base, mask_reg);
}