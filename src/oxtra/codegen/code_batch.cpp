#include "code_batch.h"
#include "jump-table/jump_table.h"
#include <spdlog/spdlog.h>

size_t codegen::CodeBatch::size() const {
	return count;
}

void codegen::CodeBatch::operator+=(utils::riscv_instruction_t inst) {
	add(inst);
}

void codegen::CodeMemory::add(utils::riscv_instruction_t inst) {
	address[count++] = inst;
}

void codegen::CodeBatchImpl::add(utils::riscv_instruction_t inst) {
	riscv[count++] = inst;
}

void codegen::CodeBatchImpl::end() {

}

void codegen::CodeBatchImpl::print() const {
	for (size_t i = 0; i < count; ++i) {
		spdlog::trace(" - instruction[{}] = {}", i, decoding::parse_riscv(riscv[i]));
	}
}

utils::riscv_instruction_t* codegen::CodeBatchImpl::get() {
	return riscv;
}

void codegen::CodeBatchImpl::reset() {
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

void codegen::RiscVStep::add(utils::riscv_instruction_t inst) {
	riscv[count++] = inst;
	jump_table::jump_debug_break(*this);
}

void codegen::RiscVStep::print() const {
	for (size_t i = 0; i < count; i += 2) {
		spdlog::trace(" - instruction[{}] = {}", i, decoding::parse_riscv(riscv[i]));
	}
}