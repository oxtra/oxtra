#ifndef OXTRA_CODE_BATCH_H
#define OXTRA_CODE_BATCH_H

#include <string>
#include <oxtra/utils/types.h>
#include "oxtra/codegen/codestore/codestore.h"
#include "oxtra/codegen/decoding/decoding.h"

namespace codegen {
	class CodeBatch {
	protected:
		utils::riscv_instruction_t riscv[codestore::max_riscv_instructions];
		size_t count;
	public:
		virtual void add(utils::riscv_instruction_t inst);

		virtual size_t size();

		virtual utils::riscv_instruction_t* get();

		virtual std::string string();

		void operator+=(utils::riscv_instruction_t inst);
	};
}


#endif //OXTRA_CODE_BATCH_H
