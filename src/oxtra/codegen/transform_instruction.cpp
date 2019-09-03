#include "transform_instruction.h"

#include "instructions/add.h"
#include "instructions/sub.h"
#include "instructions/inc.h"
#include "instructions/dec.h"
#include "instructions/mov.h"
#include "instructions/shl.h"
#include "instructions/shr.h"
#include "instructions/sar.h"
#include "instructions/imul.h"
#include "instructions/syscall.h"

using namespace codegen;
using namespace fadec;

std::unique_ptr<codegen::Instruction> codegen::transform_instruction(const fadec::Instruction& inst) {
	switch (inst.get_type()) {
		case InstructionType::ADD:
		case InstructionType::ADD_IMM:
			return std::make_unique<Add>(inst);

		case InstructionType::SUB:
		case InstructionType::SUB_IMM:
			return std::make_unique<Sub>(inst);

		case InstructionType::MOV_IMM:
		case InstructionType::MOVABS_IMM:
		case InstructionType::MOV:
			return std::make_unique<Mov>(inst);

		case InstructionType::SHL_CL:
		case InstructionType::SHL_IMM:
			return std::make_unique<Shl>(inst);

		case InstructionType::SHR_CL:
		case InstructionType::SHR_IMM:
			return std::make_unique<Shr>(inst);

		case InstructionType::SAR_CL:
		case InstructionType::SAR_IMM:
			return std::make_unique<Sar>(inst);

		case InstructionType::INC:
			return std::make_unique<Inc>(inst);

		case InstructionType::DEC:
			return std::make_unique<Dec>(inst);

		case InstructionType::IMUL2:
			return std::make_unique<Imul>(inst);

		case InstructionType::SYSCALL:
			return std::make_unique<Syscall>(inst);

		default:
			return nullptr;
	}
}