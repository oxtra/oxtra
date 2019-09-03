#include "transform_instruction.h"

#include "oxtra/codegen/instructions/arithmetic/add.h"
#include "oxtra/codegen/instructions/arithmetic/sub.h"
#include "oxtra/codegen/instructions/arithmetic/inc.h"
#include "oxtra/codegen/instructions/arithmetic/dec.h"
#include "oxtra/codegen/instructions/memory/mov.h"
#include "oxtra/codegen/instructions/memory/pop.h"
#include "oxtra/codegen/instructions/memory/push.h"
#include "oxtra/codegen/instructions/memory/movsx.h"
#include "oxtra/codegen/instructions/memory/movzx.h"
#include "oxtra/codegen/instructions/logic/shl.h"
#include "oxtra/codegen/instructions/logic/shr.h"
#include "oxtra/codegen/instructions/logic/sar.h"
#include "oxtra/codegen/instructions/arithmetic/imul.h"
#include "oxtra/codegen/instructions/control-flow/syscall.h"
#include "oxtra/codegen/instructions/control-flow/jmp.h"
#include "oxtra/codegen/instructions/control-flow/call.h"
#include "oxtra/codegen/instructions/control-flow/ret.h"


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

		case InstructionType::JMP:
		case InstructionType::JMP_IND:
			return std::make_unique<Jmp>(inst);

		case InstructionType::CALL:
		case InstructionType::CALL_IND:
			return std::make_unique<Call>(inst);

		case InstructionType::RET:
		case InstructionType::RET_IMM:
			return std::make_unique<Ret>(inst);

		case InstructionType::PUSH:
			return std::make_unique<Push>(inst);

		case InstructionType::POP:
			return std::make_unique<Pop>(inst);

		case InstructionType::MOVSX:
			return std::make_unique<Movsx>(inst);

		case InstructionType::MOVZX:
			return std::make_unique<Movzx>(inst);

		default:
			return nullptr;
	}
}