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

		size_t size() const;

		void operator+=(utils::riscv_instruction_t inst);

		virtual utils::riscv_instruction_t& operator[](size_t index) = 0;
	};

	/**
	 * Wraps a pointer of riscv code.
	 */
	class CodeMemory : public CodeBatch {
	protected:
		utils::riscv_instruction_t* address;
	public:
		explicit CodeMemory(utils::riscv_instruction_t* address)
			: address{address} {}

		virtual size_t add(utils::riscv_instruction_t inst) override;

		virtual void insert(size_t index, utils::riscv_instruction_t inst) override;

		virtual utils::riscv_instruction_t& operator[](size_t index) override;
	};

	/**
	 * Manages an internal storage of riscv code.
	 */
	class CodeStash : public CodeBatch {
	protected:
		utils::riscv_instruction_t riscv[codestore::max_riscv_instructions];
	public:
		virtual size_t add(utils::riscv_instruction_t inst) override;

		virtual void insert(size_t index, utils::riscv_instruction_t inst) override;

		virtual utils::riscv_instruction_t& operator[](size_t index)  override;

		virtual void end();

		virtual void print() const;

		utils::riscv_instruction_t* get();

		void reset();
	};

	/**
	 * Generates a call into the debugger after every x86 instruction.
	 */
	class X86Step : public CodeStash {
	public:
		void end() final;

		void print() const final;
	};

	/**
	 * Generates a call into the debugger after every riscv instruction.
	 */
	class RiscVStep : public CodeStash {
	public:
		size_t add(utils::riscv_instruction_t inst) final;

		void print() const final;
	};
}

#endif //OXTRA_CODE_BATCH_H
