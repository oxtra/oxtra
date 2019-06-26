#include <oxtra/codegen/encoding/encoding.h>
#include "oxtra/codegen/codegen.h"
#include <spdlog/spdlog.h>

using namespace codegen;
using namespace utils;
using namespace codestore;
using namespace fadec;
using namespace encoding;

CodeGenerator::CodeGenerator(const arguments::Arguments& args, const elf::Elf& elf)
		: _args{args}, _elf{elf}, _codestore{args, elf} {}

host_addr_t CodeGenerator::translate(guest_addr_t addr) {
	//const auto cached_code = _codestore.find(addr);
	//if (cached_code)
	//	return cached_code;

	//const auto next_codeblock = _codestore.get_next_block(addr);
	//if (next_codeblock == nullptr) {
	//	next_codeblock =
	//}
	uint8_t immediate = 42;
	uint8_t x86_add_instr[] = {0x48, 0x83, 0xc0, immediate};
	auto x86_add = Instruction();
	decode(reinterpret_cast<uint8_t*> (x86_add_instr), sizeof(x86_add_instr), DecodeMode::decode_64, _elf.get_base_vaddr(), x86_add);

	char formatted_string[512];
	fadec::format(x86_add, formatted_string, sizeof(formatted_string));

	spdlog::info("Fadec decoded instruction {}", formatted_string);

	auto& codeblock = _codestore.create_block();
	spdlog::info("Created codestore");
	riscv_instruction_t instructions[max_riscv_instructions];

	auto instruction_count = translate_add(x86_add, instructions);
	spdlog::info("Finished translating");
	_codestore.add_instruction(codeblock, x86_add, instructions, instruction_count);

	spdlog::info("Finished translating code block");

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



	return codeblock.riscv_start;
}

size_t
CodeGenerator::translate_add(const fadec::Instruction& x86_instruction, utils::riscv_instruction_t* riscv_instruction) {
	//TODO: check for 12 bits
	spdlog::info("Immediate {} will be loaded", static_cast<uint16_t>(x86_instruction.get_immediate()));
	riscv_instruction[0] = LUI(RiscVRegister::a0, static_cast<uint16_t>(x86_instruction.get_immediate()));
	riscv_instruction[1] = JALR(RiscVRegister::zero, RiscVRegister::ra, 0);

	return 2;
}