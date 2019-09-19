#include "transform_instruction.h"

#include "oxtra/codegen/instructions/arithmetic/add.h"
#include "oxtra/codegen/instructions/arithmetic/adc.h"
#include "oxtra/codegen/instructions/arithmetic/inc.h"
#include "oxtra/codegen/instructions/arithmetic/sub.h"
#include "oxtra/codegen/instructions/arithmetic/dec.h"
#include "oxtra/codegen/instructions/arithmetic/idiv.h"
#include "oxtra/codegen/instructions/arithmetic/mul.h"
#include "oxtra/codegen/instructions/arithmetic/neg.h"
#include "oxtra/codegen/instructions/control-flow/call.h"
#include "oxtra/codegen/instructions/control-flow/jmp.h"
#include "oxtra/codegen/instructions/control-flow/ret.h"
#include "oxtra/codegen/instructions/control-flow/syscall.h"
#include "oxtra/codegen/instructions/logic/and.h"
#include "oxtra/codegen/instructions/logic/nop.h"
#include "oxtra/codegen/instructions/logic/not.h"
#include "oxtra/codegen/instructions/logic/or.h"
#include "oxtra/codegen/instructions/logic/sar.h"
#include "oxtra/codegen/instructions/logic/shl.h"
#include "oxtra/codegen/instructions/logic/shr.h"
#include "oxtra/codegen/instructions/logic/xor.h"
#include "oxtra/codegen/instructions/logic/setcc/setcc.h"
#include "oxtra/codegen/instructions/memory/cex.h"
#include "oxtra/codegen/instructions/memory/csep.h"
#include "oxtra/codegen/instructions/memory/lea.h"
#include "oxtra/codegen/instructions/memory/leave.h"
#include "oxtra/codegen/instructions/memory/mov.h"
#include "oxtra/codegen/instructions/memory/movsx.h"
#include "oxtra/codegen/instructions/memory/movzx.h"
#include "oxtra/codegen/instructions/memory/pop.h"
#include "oxtra/codegen/instructions/memory/push.h"
#include "oxtra/codegen/instructions/control-flow/conditional-jumps/jcc.h"
#include "oxtra/codegen/instructions/memory/cmov/cmov.h"
#include "oxtra/codegen/instructions/comparable/cmp.h"
#include "oxtra/codegen/instructions/comparable/test.h"
#include "oxtra/codegen/instructions/flags/clc.h"
#include "oxtra/codegen/instructions/flags/stc.h"
#include "oxtra/codegen/instructions/string/cmps.h"
#include "oxtra/codegen/instructions/string/lods.h"
#include "oxtra/codegen/instructions/string/movs.h"
#include "oxtra/codegen/instructions/string/scas.h"
#include "oxtra/codegen/instructions/string/stos.h"

using namespace codegen;
using namespace fadec;

std::unique_ptr<codegen::Instruction> codegen::transform_instruction(const fadec::Instruction& inst) {
	switch (inst.get_type()) {
		case InstructionType::ADD:
		case InstructionType::ADD_IMM:
			return std::make_unique<Add>(inst);
		case InstructionType::JZ:
			return std::make_unique<Jz>(inst);
		case InstructionType::JO:
			return std::make_unique<Jo>(inst);
		case InstructionType::JC:
			return std::make_unique<Jc>(inst);
		case InstructionType::JS:
			return std::make_unique<Js>(inst);
		case InstructionType::JP:
			return std::make_unique<Jp>(inst);
		case InstructionType::JNZ:
			return std::make_unique<Jnz>(inst);
		case InstructionType::JNO:
			return std::make_unique<Jno>(inst);
		case InstructionType::JNC:
			return std::make_unique<Jnc>(inst);
		case InstructionType::JNS:
			return std::make_unique<Jns>(inst);
		case InstructionType::JNP:
			return std::make_unique<Jnp>(inst);
		case InstructionType::JA:
			return std::make_unique<Ja>(inst);
		case InstructionType::JBE:
			return std::make_unique<Jbe>(inst);
		case InstructionType::JG:
			return std::make_unique<Jg>(inst);
		case InstructionType::JGE:
			return std::make_unique<Jge>(inst);
		case InstructionType::JL:
			return std::make_unique<Jl>(inst);
		case InstructionType::JLE:
			return std::make_unique<Jle>(inst);
		case InstructionType::JCXZ:
			return std::make_unique<Jcxz>(inst);

		case InstructionType::CMOVA:
			return std::make_unique<Cmova>(inst);
		case InstructionType::CMOVBE:
			return std::make_unique<Cmovbe>(inst);
		case InstructionType::CMOVC:
			return std::make_unique<Cmovc>(inst);
		case InstructionType::CMOVG:
			return std::make_unique<Cmovg>(inst);
		case InstructionType::CMOVGE:
			return std::make_unique<Cmovge>(inst);
		case InstructionType::CMOVL:
			return std::make_unique<Cmovl>(inst);
		case InstructionType::CMOVLE:
			return std::make_unique<Cmovle>(inst);
		case InstructionType::CMOVNC:
			return std::make_unique<Cmovnc>(inst);
		case InstructionType::CMOVNO:
			return std::make_unique<Cmovno>(inst);
		case InstructionType::CMOVNP:
			return std::make_unique<Cmovnp>(inst);
		case InstructionType::CMOVNS:
			return std::make_unique<Cmovns>(inst);
		case InstructionType::CMOVNZ:
			return std::make_unique<Cmovnz>(inst);
		case InstructionType::CMOVO:
			return std::make_unique<Cmovo>(inst);
		case InstructionType::CMOVP:
			return std::make_unique<Cmovp>(inst);
		case InstructionType::CMOVS:
			return std::make_unique<Cmovs>(inst);
		case InstructionType::CMOVZ:
			return std::make_unique<Cmovz>(inst);

		case InstructionType::SETA:
			return std::make_unique<Seta>(inst);
		case InstructionType::SETBE:
			return std::make_unique<Setbe>(inst);
		case InstructionType::SETC:
			return std::make_unique<Setc>(inst);
		case InstructionType::SETG:
			return std::make_unique<Setg>(inst);
		case InstructionType::SETGE:
			return std::make_unique<Setge>(inst);
		case InstructionType::SETL:
			return std::make_unique<Setl>(inst);
		case InstructionType::SETLE:
			return std::make_unique<Setle>(inst);
		case InstructionType::SETNC:
			return std::make_unique<Setnc>(inst);
		case InstructionType::SETNO:
			return std::make_unique<Setno>(inst);
		case InstructionType::SETNP:
			return std::make_unique<Setnp>(inst);
		case InstructionType::SETNS:
			return std::make_unique<Setns>(inst);
		case InstructionType::SETNZ:
			return std::make_unique<Setnz>(inst);
		case InstructionType::SETO:
			return std::make_unique<Seto>(inst);
		case InstructionType::SETP:
			return std::make_unique<Setp>(inst);
		case InstructionType::SETS:
			return std::make_unique<Sets>(inst);
		case InstructionType::SETZ:
			return std::make_unique<Setz>(inst);

		case InstructionType::CMP:
		case InstructionType::CMP_IMM:
			return std::make_unique<Cmp>(inst);

		case InstructionType::TEST:
		case InstructionType::TEST_IMM:
			return std::make_unique<Test>(inst);

		case InstructionType::CLC:
			return std::make_unique<Clc>(inst);

		case InstructionType::STC:
			return std::make_unique<Stc>(inst);

		case InstructionType::ADC:
		case InstructionType::ADC_IMM:
			return std::make_unique<Adc>(inst);

		case InstructionType::SUB:
		case InstructionType::SUB_IMM:
			return std::make_unique<Sub>(inst);

		case InstructionType::DEC:
			return std::make_unique<Dec>(inst);
		case InstructionType::INC:
			return std::make_unique<Inc>(inst);

		case InstructionType::NEG:
			return std::make_unique<Neg>(inst);

		case InstructionType::MUL:
		case InstructionType::IMUL:
		case InstructionType::IMUL2:
		case InstructionType::IMUL3:
			return std::make_unique<Mul>(inst);

		case InstructionType::DIV:
		case InstructionType::IDIV:
			return std::make_unique<Idiv>(inst);

		case InstructionType::MOV_IMM:
		case InstructionType::MOVABS_IMM:
		case InstructionType::MOV:
			return std::make_unique<Mov>(inst);

		case InstructionType::AND:
		case InstructionType::AND_IMM:
			return std::make_unique<And>(inst);

		case InstructionType::OR:
		case InstructionType::OR_IMM:
			return std::make_unique<Or>(inst);

		case InstructionType::NOT:
			return std::make_unique<Not>(inst);

		case InstructionType::XOR:
		case InstructionType::XOR_IMM:
			return std::make_unique<Xor>(inst);

		case InstructionType::SHL_CL:
		case InstructionType::SHL_IMM:
			return std::make_unique<Shl>(inst);

		case InstructionType::SHR_CL:
		case InstructionType::SHR_IMM:
			return std::make_unique<Shr>(inst);

		case InstructionType::SAR_CL:
		case InstructionType::SAR_IMM:
			return std::make_unique<Sar>(inst);

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

		case InstructionType::LEAVE:
			return std::make_unique<Leave>(inst);

		case InstructionType::PUSH:
			return std::make_unique<Push>(inst);

		case InstructionType::POP:
			return std::make_unique<Pop>(inst);

		case InstructionType::MOVSX:
			return std::make_unique<Movsx>(inst);

		case InstructionType::MOVZX:
			return std::make_unique<Movzx>(inst);

		case InstructionType::NOP:
		case InstructionType::FNOP:
			return std::make_unique<Nop>(inst);

		case InstructionType::C_EX:
			return std::make_unique<Cex>(inst);

		case InstructionType::C_SEP:
			return std::make_unique<CSep>(inst);

		case InstructionType::LEA:
			return std::make_unique<Lea>(inst);

		case InstructionType::CMPS:
			return std::make_unique<Cmps>(inst);
		case InstructionType::LODS:
			return std::make_unique<Lods>(inst);
		case InstructionType::MOVS:
			return std::make_unique<Movs>(inst);
		case InstructionType::SCAS:
			return std::make_unique<Scas>(inst);
		case InstructionType::STOS:
			return std::make_unique<Stos>(inst);

		default:
			return nullptr;
	}
}
