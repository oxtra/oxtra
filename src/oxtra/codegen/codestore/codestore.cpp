#include "oxtra/codegen/codestore/codestore.h"

using namespace oxtra;
using namespace codegen::codestore;

CodeStore::CodeStore(const elf::Elf& elf)
		: _elf(elf), _instruction_offset_buffer(0x10000), _block_entries(0x10000), _code_buffer(0x10000) {}

oxtra::host_addr_t CodeStore::find(oxtra::guest_addr_t x86_code) const {

}

BlockEntry* CodeStore::get_next_block(oxtra::guest_addr_t x86_code) const {

}

BlockEntry& CodeStore::create_block() {

}

void CodeStore::add_instruction(BlockEntry& block, const FdInstr& x86_instruction,
								oxtra::riscv_instruction_t* riscv_instructions, size_t num_instructions) {

}