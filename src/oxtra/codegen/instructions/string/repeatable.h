#ifndef OXTRA_REPEATABLE_H
#define OXTRA_REPEATABLE_H

#include "oxtra/codegen/instruction.h"

namespace codegen {
	/**
	 * Handles the rep prefix not repz or repnz so don't use it for scas and cmps.
	 */
	class Repeatable : public Instruction {
	public:
		explicit Repeatable(const fadec::Instruction& inst)
			: Instruction{inst, flags::none, flags::none} {}

		void generate(CodeBatch& batch) const final;

		virtual void execute_operation(CodeBatch& batch) const = 0;
	};

	/**
	 * Handles the repz and repnz prefix.
	 */
	class RepeatableFlag : public Instruction {
	public:
		explicit RepeatableFlag(const fadec::Instruction& inst)
				: Instruction{inst, flags::all, flags::none} {}

		virtual void generate(CodeBatch& batch) const override;

	protected:
		virtual void execute_operation(CodeBatch& batch) const = 0;

		static constexpr auto diff_reg = encoding::RiscVRegister::t0;
		static constexpr auto src_reg = encoding::RiscVRegister::t1;
		static constexpr auto dst_reg = encoding::RiscVRegister::t2;

	private:
		void generate_loop(CodeBatch& batch, bool z) const;

		void update_flags(CodeBatch& batch) const;
	};
}

#endif //OXTRA_REPEATABLE_H