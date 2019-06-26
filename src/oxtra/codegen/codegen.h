#ifndef OXTRA_CODEGEN_H
#define OXTRA_CODEGEN_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codestore/codestore.h"
#include "oxtra/elf/elf.h"
#include <fadec.h>

namespace codegen {
	class CodeGenerator {
	private:
		const arguments::Arguments& _args;
		const elf::Elf& _elf;
		codestore::CodeStore _codestore;

	public:
		CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf);

		CodeGenerator(CodeGenerator&) = delete;

		CodeGenerator(CodeGenerator&&) = delete;

	public:
		utils::host_addr_t translate(utils::guest_addr_t addr);

		const arguments::Arguments& get_args() const;

	private:
		size_t translate_add(const fadec::Instruction& x86_instruction, utils::riscv_instruction_t* riscv_instruction);
		//size_t translate_mov(const fadec::Instruction& x86_instruction, utils::riscv_instruction_t* riscv_instruction);
	};
}

#endif //OXTRA_CODEGEN_H