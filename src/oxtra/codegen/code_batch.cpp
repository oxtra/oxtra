#include "code_batch.h"
#include "jumptable/jump_table.h"

size_t codegen::CodeBatch::size() const {
	return count;
}

void codegen::CodeBatch::operator+=(utils::riscv_instruction_t inst) {
	add(inst);
}

void codegen::CodeMemory::add(utils::riscv_instruction_t inst) {
	address[count++] = inst;
}

void codegen::CodeStash::add(utils::riscv_instruction_t inst) {
	riscv[count++] = inst;
}

void codegen::CodeStash::end() {

}

std::string codegen::CodeStash::string() const {
	std::string str{};

	for (size_t i = 0; i < count; ++i) {
		char buffer[128];
		sprintf(buffer, " - instruction[%llu] = %s", i, decoding::parse_riscv(riscv[i]).c_str());
		str += buffer;
	}

	return str;
}

utils::riscv_instruction_t* codegen::CodeStash::get() {
	return riscv;
}

void codegen::CodeStash::reset() {
	count = 0;
}

void codegen::X86Step::end() {
	jump_table::jump_debug_break(*this, encoding::RiscVRegister::ra);
}

std::string codegen::X86Step::string() const {
	std::string str{};

	for (size_t i = 0; i < count - 1; ++i) {
		char buffer[128];
		sprintf(buffer, " - instruction[%llu] = %s", i, decoding::parse_riscv(riscv[i]).c_str());
		str += buffer;
	}

	return str;
}

void codegen::RiscVStep::add(utils::riscv_instruction_t inst) {
	riscv[count++] = inst;
	jump_table::jump_debug_break(*this, encoding::RiscVRegister::ra);
}

std::string codegen::RiscVStep::string() const {
	std::string str{};

	for (size_t i = 0; i < count; i += 2) {
		char buffer[128];
		sprintf(buffer, " - instruction[%llu] = %s", i, decoding::parse_riscv(riscv[i]).c_str());
		str += buffer;
	}

	return str;
}