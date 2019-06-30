#ifndef OXTRA_RISCV_DECODING_H
#define OXTRA_RISCV_DECODING_H

#include <sstream>
#include <string>
#include <inttypes.h>
#include <oxtra/codegen/encoding/encoding.h>

/* implemented:
 *   - base-instruction-set			(ignoring FENCE, ECALL, EBREAK and CSRR-instructions)
 *   - rv64i-base-instruction-set	(done)
 *   - rv32m-standard-extension		(done)
 *   - rv64m-standard-extensions	(done)
 * */
std::string parse_riscv(utils::riscv_instruction_t instruction);


/* possible parsed formats:
 * inst src1, src2 -> dest
 * inst src, imm -> dest
 * inst $[dest-address] ? src1, src2
 *
 * */

#endif //OXTRA_RISCV_DECODING_H
