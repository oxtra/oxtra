#include "adc.h"
#include "oxtra/codegen/helper.h"

void codegen::Adc::execute_operation(CodeBatch& batch, encoding::RiscVRegister dst, encoding::RiscVRegister src) const {
	evaluate_carry(batch);

	if (get_update() & (Flags::carry | Flags::overflow)) {
		const auto [carry, overflow] = helper::calculate_entries(
				jump_table::Entry::carry_add, jump_table::Entry::overflow_add, get_operand(0).get_size());

		static_assert(static_cast<uint16_t>(jump_table::Entry::carry_adc) - static_cast<uint16_t>(jump_table::Entry::carry_add) == 4,
					  "carry_add and carry_adc have to be 4 apart.");

		static_assert(static_cast<uint16_t>(jump_table::Entry::carry_adc) - static_cast<uint16_t>(jump_table::Entry::carry_add) == 4,
					  "overflow_add and overflow_adc have to be 4 apart.");

		// calculate the offset into the jump table based on the status of the carry flag
		// carry clear -> add, carry set -> adc
		batch += encoding::SLLI(encoding::RiscVRegister::t5, encoding::RiscVRegister::t4, 4);

		if (get_update() & Flags::overflow) {
			batch += encoding::ADDI(encoding::RiscVRegister::t6, encoding::RiscVRegister::t5,
									static_cast<uint16_t>(overflow) * 4);
			update_overflow(batch, encoding::RiscVRegister::t6, src, dst);
		}

		batch += encoding::ADD(dst, src, dst);
		batch += encoding::ADD(dst, encoding::RiscVRegister::t4, dst);

		if (get_update() & Flags::carry) {
			batch += encoding::ADDI(encoding::RiscVRegister::t4,encoding::RiscVRegister::t5,
					static_cast<uint16_t>(carry) * 4);
			update_carry(batch, encoding::RiscVRegister::t4, src, dst);
		}
	}
	else {
		batch += encoding::ADD(dst, src, dst);
		batch += encoding::ADD(dst, encoding::RiscVRegister::t4, dst);
	}
}