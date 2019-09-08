#include "code_batch.h"
#include "jump-table/jump_table.h"
#include "oxtra/dispatcher/dispatcher.h"
#include <spdlog/spdlog.h>

size_t codegen::CodeBatch::size() const {
	return count;
}

void codegen::CodeBatch::operator+=(utils::riscv_instruction_t inst) {
	add(inst);
}

size_t codegen::CodeMemory::add(utils::riscv_instruction_t inst) {
	if(count >= max_size)
		dispatcher::Dispatcher::fault_exit("CodeMemory::add - buffer-overflow");
	address[count] = inst;
	return count++;
}

void codegen::CodeMemory::insert(size_t index, utils::riscv_instruction_t inst) {
	address[index] = inst;
}

utils::riscv_instruction_t& codegen::CodeMemory::operator[](size_t index) {
	return address[index];
}

size_t codegen::CodeBatchImpl::add(utils::riscv_instruction_t inst) {
	riscv[count] = inst;
	return count++;
}

void codegen::CodeBatchImpl::insert(size_t index, utils::riscv_instruction_t inst) {
	riscv[index] = inst;
}

utils::riscv_instruction_t& codegen::CodeBatchImpl::operator[](size_t index) {
	return riscv[index];
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