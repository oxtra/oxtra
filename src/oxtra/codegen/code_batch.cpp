#include "code_batch.h"
#include "jump-table/jump_table.h"
#include <spdlog/spdlog.h>

size_t codegen::CodeBatch::size() const {
	return count;
}

void codegen::CodeBatch::operator+=(utils::riscv_instruction_t inst) {
	add(inst);
}

size_t codegen::CodeMemory::add(utils::riscv_instruction_t inst) {
	address[count] = inst;
	return count++;
}

void codegen::CodeMemory::insert(size_t index, utils::riscv_instruction_t inst) {
	address[index] = inst;
}

utils::riscv_instruction_t& codegen::CodeMemory::operator[](size_t index) {
	return address[index];
}

size_t codegen::CodeStash::add(utils::riscv_instruction_t inst) {
	riscv[count] = inst;
	return count++;
}

void codegen::CodeStash::insert(size_t index, utils::riscv_instruction_t inst) {
	riscv[index] = inst;
}

utils::riscv_instruction_t& codegen::CodeStash::operator[](size_t index) {
	return riscv[index];
}

void codegen::CodeStash::end() {

}

void codegen::CodeStash::print() const {
	for (size_t i = 0; i < count; ++i) {
		spdlog::trace(" - instruction[{}] = {}", i, decoding::parse_riscv(riscv[i]));
	}
}

utils::riscv_instruction_t* codegen::CodeStash::get() {
	return riscv;
}

void codegen::CodeStash::reset() {
	count = 0;
}

void codegen::X86Step::end() {
	jump_table::jump_debug_break(*this);
}

void codegen::X86Step::print() const {
	for (size_t i = 0; i < count - 1; ++i) {
		spdlog::trace(" - instruction[{}] = {}", i, decoding::parse_riscv(riscv[i]));
	}
}

size_t codegen::RiscVStep::add(utils::riscv_instruction_t inst) {
	size_t index = count;
	riscv[count++] = inst;
	jump_table::jump_debug_break(*this);
	return index;
}

void codegen::RiscVStep::print() const {
	for (size_t i = 0; i < count; i += 2) {
		spdlog::trace(" - instruction[{}] = {}", i, decoding::parse_riscv(riscv[i]));
	}
}