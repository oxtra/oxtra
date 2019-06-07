#include "oxtra/codegen/codegen.h"

codegen::CodeGenerator::CodeGenerator(const elf::Elf &elf)
	: _codestore{elf} {}

oxtra::host_addr_t codegen::CodeGenerator::translate(oxtra::guest_addr_t addr) {
	const auto cached_code = _codestore.find(addr);
	if (cached_code)
		return cached_code;

	const auto next_codeblock = _codestore.get_next_block(addr);

	/*
	 * max block size = min(next_codeblock.start, instruction offset limit)
	 *
	 * loop over instructions and decode
	 * - big switch for each instruction type
	 * - translate instruction
	 * - add risc-v instructions into code store
	 *
	 * add jump to dispatcher::host_enter
	 * return address to translated code
	 */

	return 0;
}
