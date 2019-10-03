#ifndef OXTRA_CODE_BATCH_H
#define OXTRA_CODE_BATCH_H

#include <string>
#include <oxtra/utils/types.h>
#include "oxtra/codegen/codestore/codestore.h"
#include "oxtra/codegen/decoding/decoding.h"

namespace codegen {
	/**
	 * Interface for writing code.
	 */
	class CodeBatch {
	protected:
		size_t count = 0;

	public:
		virtual size_t add(utils::riscv_instruction_t inst) = 0;

		virtual void insert(size_t index, utils::riscv_instruction_t inst) = 0;

		virtual size_t offset(size_t start, size_t end) = 0;

		size_t size() const;

		void operator+=(utils::riscv_instruction_t inst);
	};

	/**
	 * Wraps a pointer of riscv code.
	 */
	class CodeMemory : public CodeBatch {
	protected:
		utils::riscv_instruction_t* address;
		size_t max_size;
	public:
		explicit CodeMemory(utils::riscv_instruction_t* address, size_t buffer_size)
				: address{address}, max_size{buffer_size} {}

		virtual size_t add(utils::riscv_instruction_t inst) override;

		virtual void insert(size_t index, utils::riscv_instruction_t inst) override;

		virtual size_t offset(size_t start, size_t end) override;
	};

	/**
	 * Manages an internal storage of riscv code.
	 */
	class CodeBatchImpl : public CodeBatch {
	protected:
		utils::riscv_instruction_t riscv[codestore::max_riscv_instructions];
	public:
		virtual size_t add(utils::riscv_instruction_t inst) override;

		virtual size_t offset(size_t start, size_t end);

		virtual void insert(size_t index, utils::riscv_instruction_t inst) override;

		virtual void reset();

		virtual void end();

		virtual void print() const;

		utils::riscv_instruction_t* get();
	};
}

#endif //OXTRA_CODE_BATCH_H
