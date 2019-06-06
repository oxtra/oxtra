as #ifndef OXTRA_CODEGEN_H
#define OXTRA_CODEGEN_H

#include "oxtra/types.h"
#include "oxtra/codegen/codestore/codestore.h"

namespace codegen {
	class CodeGenerator {
	private:
		codestore::CodeStore _codestore;

	public:
		CodeGenerator(CodeGenerator &) = delete;

		CodeGenerator(CodeGenerator &&) = delete;

	public:
		oxtra::real_t translate(oxtra::virt_t addr);
	};
}

#endif //OXTRA_CODEGEN_H
