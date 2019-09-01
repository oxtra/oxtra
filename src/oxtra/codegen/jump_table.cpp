#include "jump_table.h"

void codegen::jump_table::jump_syscall_handler(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = encoding::JALR(return_address, CodeGenerator::jump_table_address, 0);
}

void codegen::jump_table::jump_reroute_static(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = encoding::JALR(return_address, CodeGenerator::jump_table_address, 4);
}

void codegen::jump_table::jump_reroute_dynamic(encoding::RiscVRegister return_address, utils::riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = encoding::JALR(return_address, CodeGenerator::jump_table_address, 8);
}

void codegen::jump_table::jump_flag_evaluation(encoding::RiscVRegister offset, encoding::RiscVRegister return_address,
						  utils::riscv_instruction_t* riscv, size_t& count) {
	riscv[count++] = encoding::ADD(offset, CodeGenerator::jump_table_address, offset);
	riscv[count++] = encoding::JALR(return_address, offset, 12);
}