#include "transform_instruction.h"

#include "instructions/add.h"

using namespace codegen;
using namespace fadec;

std::unique_ptr<codegen::Instruction> codegen::transform_instruction(const fadec::Instruction& inst) {
	switch (inst.get_type()) {
		case InstructionType::ADD:
		case InstructionType::ADD_IMM:
			return std::make_unique<Add>(inst);

		default:
			return nullptr;
	}
}