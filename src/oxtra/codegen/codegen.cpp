#include "oxtra/codegen/codegen.h"

using namespace codegen;
using namespace utils;

CodeGenerator::CodeGenerator(const elf::Elf& elf, const arguments::Arguments& args)
		: _codestore{elf, args}, _args{args} {}

host_addr_t CodeGenerator::translate(guest_addr_t addr) {
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
