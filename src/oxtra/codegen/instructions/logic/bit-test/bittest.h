#ifndef OXTRA_BITTEST_H
#define OXTRA_BITTEST_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	class BitTest : public codegen::Instruction {
		friend class Bt;
	public:
		explicit BitTest(const fadec::Instruction& inst)
				: codegen::Instruction{inst, flags::carry, flags::none} {}

		void generate(CodeBatch& batch) const final;

	protected:
		virtual void manipulate_bit(CodeBatch& batch, encoding::RiscVRegister bit_base) const = 0;

		static constexpr auto
			bit_value_reg = encoding::RiscVRegister::t1,
			mask_reg = encoding::RiscVRegister::t2;

	private:
		static uint16_t get_shift_amount(uint8_t size, uintptr_t imm);
	};
}

#include "bt.h"
#include "btc.h"
#include "btr.h"
#include "bts.h"

#endif //OXTRA_BITTEST_H
