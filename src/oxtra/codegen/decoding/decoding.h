#ifndef OXTRA_DECODING_H
#define OXTRA_DECODING_H

#include <sstream>
#include <string>
#include <inttypes.h>
#include <oxtra/codegen/encoding/encoding.h>

/* implemented:
 *   - base-instruction-set			(ignoring FENCE, EBREAK and CSRR-instructions)
 *   - rv64i-base-instruction-set	(done)
 *   - rv32m-standard-extension		(done)
 *   - rv64m-standard-extensions	(done)
 * */
namespace decoding {
	std::string parse_riscv(utils::riscv_instruction_t instruction);
}

#endif //OXTRA_DECODING_H
