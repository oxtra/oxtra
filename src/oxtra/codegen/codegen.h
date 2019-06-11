#ifndef OXTRA_CODEGEN_H
#define OXTRA_CODEGEN_H

#include "oxtra/types.h"
#include "oxtra/codegen/codestore/codestore.h"
#include "oxtra/elf/elf.h"

extern "C" {
#include "fadec.h"
}

namespace codegen {
	class CodeGenerator {
	private:
		codestore::CodeStore _codestore;

	public:
		CodeGenerator(const elf::Elf& elf);

		CodeGenerator(CodeGenerator&) = delete;

		CodeGenerator(CodeGenerator&&) = delete;

	public:
		oxtra::host_addr_t translate(oxtra::guest_addr_t addr);
	};
}

#endif //OXTRA_CODEGEN_H