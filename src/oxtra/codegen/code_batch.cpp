#include "code_batch.h"
#include "oxtra/logger/logger.h"
#include "oxtra/dispatcher/dispatcher.h"

size_t codegen::CodeBatch::size() const {
	return count;
}

void codegen::CodeBatch::operator+=(utils::riscv_instruction_t inst) {
	add(inst);
}

size_t codegen::CodeMemory::add(utils::riscv_instruction_t inst) {
	if (count >= max_size)
		dispatcher::Dispatcher::fault_exit("CodeMemory::add - buffer-overflow");
	address[count] = inst;
	return count++;
}

size_t codegen::CodeMemory::offset(size_t start, size_t end) {
	return end - start;
}

void codegen::CodeMemory::insert(size_t index, utils::riscv_instruction_t inst) {
	address[index] = inst;
}

size_t codegen::CodeBatchImpl::add(utils::riscv_instruction_t inst) {
	if (count >= codestore::max_riscv_instructions)
		dispatcher::Dispatcher::fault_exit("CodeBatchImpl::add - buffer-overflow");
	riscv[count] = inst;
	return count++;
}

size_t codegen::CodeBatchImpl::offset(size_t start, size_t end) {
	return end - start;
}

void codegen::CodeBatchImpl::insert(size_t index, utils::riscv_instruction_t inst) {
	riscv[index] = inst;
}

void codegen::CodeBatchImpl::reset() {
	count = 0;
}

void codegen::CodeBatchImpl::end() {
}

void codegen::CodeBatchImpl::print() const {
	for (size_t i = 0; i < count; ++i)
		logger::log(logger::Level::riscv, "    [{:02}] = {}\n", i, decoding::parse_riscv(riscv[i]));
}

utils::riscv_instruction_t* codegen::CodeBatchImpl::get() {
	return riscv;
}