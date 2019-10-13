#ifndef OXTRA_CODEGEN_H
#define OXTRA_CODEGEN_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codestore/codestore.h"
#include "instruction.h"
#include "oxtra/elf/elf.h"

namespace codegen {
	struct CallEntry {
		CallEntry() = default;
		CallEntry(uintptr_t x86, uintptr_t riscv)
			: x86_address{x86}, riscv_address{riscv} {}

		uintptr_t x86_address;
		uintptr_t riscv_address;
	};

	class CodeGenerator {
		friend class Call;
	private:
		const elf::Elf& _elf;
		codestore::CodeStore _codestore;
		std::unique_ptr<codegen::CodeBatchImpl> _batch;
		std::vector<CallEntry> _call_table;

	public:
		CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf);

		CodeGenerator(const CodeGenerator&) = delete;

		CodeGenerator(CodeGenerator&&) = delete;

	public:
		utils::host_addr_t translate(utils::guest_addr_t addr);

		void update_basic_block(utils::host_addr_t addr, utils::host_addr_t absolute_address);

		CallEntry* get_call_table();

	private:
		using inst_vec_t = std::vector<std::unique_ptr<codegen::Instruction>>;

		codegen::Instruction& decode_instruction(utils::guest_addr_t& addr, inst_vec_t& inst_vec) const;

		size_t recursive_flag_requirements(size_t require, uintptr_t addr, uint8_t depth) const;

		/**
	 	 * Transforms a fadec instruction object into our own representation.
	 	 * @param inst The fadec instruction object.
	 	 * @return A pointer to our instruction object.
	 	 */
		std::unique_ptr<codegen::Instruction> transform_instruction(const fadec::Instruction& inst) const;
	};
}

#endif //OXTRA_CODEGEN_H
