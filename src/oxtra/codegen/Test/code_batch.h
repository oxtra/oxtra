#ifndef OXTRA_CODE_BATCH_H
#define OXTRA_CODE_BATCH_H

#include <string>
#include <oxtra/utils/types.h>
#include "oxtra/codegen/codestore/codestore.h"
#include "oxtra/codegen/decoding/decoding.h"

namespace codegen {
	/*
	 * string:
	 * for (size_t j = 0; j < count; j++) {
			spdlog::trace(" - instruction[{}] = {}", j, decoding::parse_riscv(riscv[j]));
		}
	 */

	class CodeBatch {
	protected:
		utils::riscv_instruction_t riscv[codestore::max_riscv_instructions];
		size_t count;
	public:
		virtual void add(utils::riscv_instruction_t inst);

		virtual void end();

		virtual std::string string() const;

		size_t size() const;

		utils::riscv_instruction_t* get();

		void reset();

		void operator+=(utils::riscv_instruction_t inst);
	};

	class X86Step : public CodeBatch {
	public:
		void end() final;

		std::string string() const final;
	};

	class RiscVStep : public CodeBatch {
	public:
		void add(utils::riscv_instruction_t inst) final;

		std::string string() const final;
	};
}


#endif //OXTRA_CODE_BATCH_H
