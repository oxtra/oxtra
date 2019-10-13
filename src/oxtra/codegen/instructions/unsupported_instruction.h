#ifndef OXTRA_UNSUPPORTED_INSTRUCTION_H
#define OXTRA_UNSUPPORTED_INSTRUCTION_H

#include "oxtra/codegen/instruction.h"
#include "oxtra/codegen/helper.h"

namespace codegen {
	class UnsupportedInstruction : public codegen::Instruction {
	public:
		explicit UnsupportedInstruction(const fadec::Instruction& inst)
			: codegen::Instruction{inst, flags::all, flags::none, true} {}

		void generate(CodeBatch& batch) const final;

		static void handle_unsupported_instruction(utils::guest_addr_t addr);
	};
}

#endif //OXTRA_UNSUPPORTED_INSTRUCTION_H
