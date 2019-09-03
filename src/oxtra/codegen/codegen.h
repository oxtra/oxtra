#ifndef OXTRA_CODEGEN_H
#define OXTRA_CODEGEN_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codestore/codestore.h"
#include "instruction.h"
#include "oxtra/elf/elf.h"

namespace codegen {
	class CodeGenerator {
	private:
		const arguments::Arguments& _args;
		const elf::Elf& _elf;
		codestore::CodeStore _codestore;

	public:
		CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf);

		CodeGenerator(const CodeGenerator&) = delete;

		CodeGenerator(CodeGenerator&&) = delete;

	public:
		utils::host_addr_t translate(utils::guest_addr_t addr);

		void update_basic_block(utils::host_addr_t addr, utils::host_addr_t absolute_address);

	private:
		using inst_vec_t = std::vector<std::unique_ptr<codegen::Instruction>>;

		codegen::Instruction& decode_instruction(utils::guest_addr_t& addr, inst_vec_t& inst_vec) const;
	};
}

#endif //OXTRA_CODEGEN_H
