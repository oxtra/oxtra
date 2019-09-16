#include <oxtra/dispatcher/dispatcher.h>
#include "instruction.h"
#include "oxtra/codegen/helper.h"

using namespace encoding;
using namespace fadec;
using namespace codegen::helper;

codegen::Instruction::Instruction(const fadec::Instruction& inst, uint8_t update, uint8_t require, bool eob)
		: fadec::Instruction{inst} {
	update_flags = update;
	require_flags = require;
	end_of_block = eob;
}

uint8_t codegen::Instruction::get_require() const {
	return require_flags;
}

uint8_t codegen::Instruction::get_update() const {
	return update_flags;
}

bool codegen::Instruction::get_eob() const {
	return end_of_block;
}

void codegen::Instruction::set_update(uint8_t flags) {
	update_flags = flags;
}

std::string codegen::Instruction::string() const {
	char buffer[256];
	fadec::format(*this, buffer, sizeof(buffer));

	return buffer;
}

RiscVRegister codegen::Instruction::translate_operand(CodeBatch& batch, size_t index, RiscVRegister* address,
													  RiscVRegister temp_a, RiscVRegister temp_b, bool modifiable,
													  bool full_load, bool sign_extend, bool destination) const {
	// extract the operand
	auto& operand = get_operand(index);

	// check if the operand is a memory-access
	if (operand.get_type() == OperandType::mem) {
		if (address)
			address[0] = read_from_memory(batch, index, temp_a, temp_b, sign_extend);
		else
			read_from_memory(batch, index, temp_a, temp_b, sign_extend);
		return temp_a;
	}

	// clear the address-register
	if (address)
		address[0] = RiscVRegister::zero;

	// check if the operand describes a register
	if (operand.get_type() == OperandType::reg) {
		// extract the mapped register
		const bool high_reg = operand.get_register_type() == RegisterType::gph;
		const auto mapped_reg = (high_reg ? map_reg_high : map_reg)(operand.get_register());

		// extract the register
		if (destination && operand.get_size() >= 4) {
			if (!full_load || operand.get_size() == 8)
				return mapped_reg;
		}
		return helper::load_from_register(batch, mapped_reg, high_reg ? 0 : operand.get_size(),
										  temp_a, modifiable, full_load, sign_extend);
	}

	// check if the operand describes an immediate
	if (operand.get_type() == OperandType::imm) {
		load_immediate(batch, get_immediate(), temp_a);
		return temp_a;
	}

	// If this point is reached, the operand-type is unknown
	return RiscVRegister::zero;
}

void codegen::Instruction::translate_destination(CodeBatch& batch, RiscVRegister reg, RiscVRegister address,
												 RiscVRegister temp_a, RiscVRegister temp_b) const {
	auto& operand = get_operand(0);

	// check if the destination is a register
	if (operand.get_type() == OperandType::reg) {
		// extract the mapped register
		const bool high_reg = operand.get_register_type() == RegisterType::gph;
		const auto mapped_reg = (high_reg ? map_reg_high : map_reg)(operand.get_register());

		// write the register to the destination
		if (operand.get_size() != 8 || mapped_reg != reg)
			move_to_register(batch, mapped_reg, reg, high_reg ? 0 : operand.get_size(), temp_a, false);
		return;
	}

	// check if the destination is a memory-operation
	if (operand.get_type() != OperandType::mem)
		return;

	// write the value to memory
	write_to_memory(batch, 0, reg, temp_a, temp_b, address);
}

RiscVRegister codegen::Instruction::translate_memory(CodeBatch& batch, size_t index, RiscVRegister temp_a,
													 RiscVRegister temp_b) const {
	if (get_address_size() < 4)
		dispatcher::Dispatcher::fault_exit("invalid addressing-size");
	const auto& operand = get_operand(index);

	// extract the displacement-mask
	const uintptr_t disp_mask = get_address_size() == 8 ? 0xffffffffffffffffull : 0x00000000ffffffffull;
	const auto displacement = get_displacement();
	const auto operation_displacement = static_cast<uintptr_t>(displacement) & disp_mask;

	// analyze the input (0 = doesn't exist, 1 = optimizable, 2 = unoptimizable)
	uint8_t disp_exists = displacement == 0 ? 0 : (displacement >= -0x800 && displacement < 0x800 ? 1 : 2);
	uint8_t index_exists = get_index_register() == fadec::Register::none ? 0 : (get_index_scale() == 0 ? 1 : 2);
	uint8_t base_exists = operand.get_register() == fadec::Register::none ? 0 : (get_address_size() == 8 && disp_exists == 0 &&
																				 index_exists == 0 ? 1 : 2);

	// check if zero is addressed
	if (disp_exists == 0 && index_exists == 0 && base_exists == 0)
		return RiscVRegister::zero;

	// extract the registers
	const auto base_reg = base_exists ? map_reg(operand.get_register()) : RiscVRegister::zero;
	const auto index_reg = index_exists ? map_reg(get_index_register()) : RiscVRegister::zero;

	// swap the registers, if one of them is the source-registers
	bool swapped = false;
	if (base_reg == temp_a || index_reg == temp_b) {
		auto temp = temp_a;
		temp_a = temp_b;
		temp_b = temp;
		swapped = true;
	}

	// build the output
	auto result = temp_a;
	if (base_exists) {
		// [base + ?]
		if (disp_exists == 1) {
			// [base + sDisp + ?]
			if (index_exists == 0)
				batch += encoding::ADDI(temp_a, base_reg, operation_displacement);
			else if (index_exists == 1) {
				// [base + sDisp + index]
				batch += encoding::ADDI(temp_b, base_reg, operation_displacement);
				batch += encoding::ADD(temp_a, temp_b, index_reg);
			} else if (index_exists == 2) {
				// [base + sDisp + index * n]
				batch += encoding::SLLI(temp_b, index_reg, get_index_scale());
				batch += encoding::ADD(temp_b, base_reg, temp_b);
				batch += encoding::ADDI(temp_a, temp_b, operation_displacement);
			}
		} else if (index_exists == 1) {
			// [base + index*1 + ?]
			if (disp_exists) {
				// [base + index*1 + lDisp]
				helper::load_immediate(batch, operation_displacement, temp_b);
				batch += encoding::ADD(temp_b, map_reg(operand.get_register()), temp_b);
				batch += encoding::ADD(temp_a, map_reg(get_index_register()), temp_b);
			} else
				batch += encoding::ADD(temp_a, map_reg(operand.get_register()), map_reg(get_index_register()));
		} else if (index_exists == 0 && disp_exists == 0)
			// [base]
			if (base_exists == 1)
				result = map_reg(operand.get_register());
			else
				batch += encoding::ADD(temp_a, map_reg(operand.get_register()), RiscVRegister::zero);
		else {
			// nothing can be optimized
			auto temp = RiscVRegister::zero;
			if (index_exists) {
				batch += encoding::SLLI(temp_b, map_reg(get_index_register()), get_index_scale());
				temp = temp_b;
			}
			batch += encoding::ADD(temp_a, temp, map_reg(operand.get_register()));
			if (disp_exists) {
				helper::load_immediate(batch, operation_displacement, temp_b);
				batch += encoding::ADD(temp_a, temp_a, temp_b);
			}
		}
	} else if (index_exists && disp_exists)
		if (index_exists == 1 && disp_exists == 1)
			// [index*1 + sDisp]
			batch += encoding::ADDI(temp_a, map_reg(get_index_register()), operation_displacement);
		else {
			// [index*n + lDisp]
			batch += encoding::SLLI(temp_a, map_reg(get_index_register()), get_index_scale());
			helper::load_immediate(batch, operation_displacement, temp_b);
			batch += encoding::ADD(temp_a, temp_a, temp_b);
		}
	else if (index_exists) {
		// [index*n]
		if (get_address_size() == 8 && index_exists == 1)
			result = map_reg(get_index_register());
		else if (index_exists == 1)
			batch += encoding::ADD(temp_a, RiscVRegister::zero, map_reg(get_index_register()));
		else
			batch += encoding::SLLI(temp_a, map_reg(get_index_register()), get_index_scale());
	} else
		// [disp]
		load_immediate(batch, operation_displacement, temp_a);

	// update the result-register
	// check if the addressing-mode is a 32-bit mode
	if (get_address_size() == 4 && (index_exists || base_exists)) {
		auto dest = (swapped && result == temp_a ? temp_b : result);
		batch += encoding::SLLI(dest, result, 32);
		batch += encoding::SRLI(dest, dest, 32);
	} else if (swapped && result == temp_a) {
		batch += encoding::MV(temp_b, result);
		return temp_b;
	}
	return result;
}

RiscVRegister codegen::Instruction::read_from_memory(CodeBatch& batch, size_t index, encoding::RiscVRegister dest,
													 encoding::RiscVRegister temp,
													 bool sign_extended) const {
	if (get_address_size() < 4)
		dispatcher::Dispatcher::fault_exit("invalid addressing-size");
	const auto& operand = get_operand(index);

	// extract the displacement-mask
	const uintptr_t disp_mask = get_address_size() == 8 ? 0xffffffffffffffffull : 0x00000000ffffffffull;
	const auto displacement = get_displacement();
	auto operation_displacement = static_cast<uintptr_t>(displacement) & disp_mask;

	// check if the memory-access is simple and compute the address
	RiscVRegister base;
	if (displacement >= -0x800 && displacement < 0x800) {
		base = RiscVRegister::zero;
		// check if a base exists
		if (operand.get_register() != Register::none)
			base = map_reg(operand.get_register());

		// check if an index exists
		if (get_index_register() != Register::none) {
			if (base != RiscVRegister::zero) {
				if (get_index_scale() == 0)
					batch += encoding::ADD(temp, base, map_reg(get_index_register()));
				else {
					batch += encoding::SLLI(temp, map_reg(get_index_register()), get_index_scale());
					batch += encoding::ADD(temp, temp, base);
				}
				base = temp;
			} else if (get_index_scale() == 0)
				base = map_reg(get_index_register());
			else {
				batch += encoding::SLLI(temp, map_reg(get_index_register()), get_index_scale());
				base = temp;
			}
		}
	} else {
		base = translate_memory(batch, index, temp, dest);
		operation_displacement = 0;
	}

	// handle segment override
	handle_segment_override(batch, base, base == temp ? dest : temp);

	// generate the memory-access
	switch (operand.get_size()) {
		case 8:
			batch += encoding::LD(dest, base, operation_displacement);
			break;
		case 4:
			batch += (sign_extended ? encoding::LW : encoding::LWU)(dest, base, operation_displacement);
			break;
		case 2:
			batch += (sign_extended ? encoding::LH : encoding::LHU)(dest, base, operation_displacement);
			break;
		case 1:
		default:
			batch += (sign_extended ? encoding::LB : encoding::LBU)(dest, base, operation_displacement);
			break;
	}
	return base;
}

void codegen::Instruction::write_to_memory(CodeBatch& batch, size_t index, encoding::RiscVRegister src,
										   encoding::RiscVRegister temp_a, encoding::RiscVRegister temp_b,
										   encoding::RiscVRegister address) const {
	if (get_address_size() < 4)
		dispatcher::Dispatcher::fault_exit("invalid addressing-size");
	const auto& operand = get_operand(index);

	// extract the displacement-mask
	const uintptr_t disp_mask = get_address_size() == 8 ? 0xffffffffffffffffull : 0x00000000ffffffffull;
	const auto displacement = get_displacement();
	auto operation_displacement = static_cast<uintptr_t>(displacement) & disp_mask;

	// check if the memory-access is simple and compute the address
	if (address == encoding::RiscVRegister::zero) {
		if (displacement >= -0x800 && displacement < 0x800) {
			// check if a base exists
			if (operand.get_register() != Register::none)
				address = map_reg(operand.get_register());

			// check if an index exists
			if (get_index_register() != Register::none) {
				if (address != RiscVRegister::zero) {
					if (get_index_scale() == 0)
						batch += encoding::ADD(temp_a, address, map_reg(get_index_register()));
					else {
						batch += encoding::SLLI(temp_b, map_reg(get_index_register()), get_index_scale());
						batch += encoding::ADD(temp_a, temp_b, address);
					}
					address = temp_a;
				} else if (get_index_scale() == 0)
					address = map_reg(get_index_register());
				else {
					batch += encoding::SLLI(temp_a, map_reg(get_index_register()), get_index_scale());
					address = temp_a;
				}
			}
		} else {
			address = translate_memory(batch, index, temp_a, temp_b);
			operation_displacement = 0;
		}
	}

	handle_segment_override(batch, address, address == temp_a ? temp_b : temp_a);

	// generate the memory-access
	switch (operand.get_size()) {
		case 8:
			batch += encoding::SD(address, src, operation_displacement);
			break;
		case 4:
			batch += encoding::SW(address, src, operation_displacement);
			break;
		case 2:
			batch += encoding::SH(address, src, operation_displacement);
			break;
		case 1:
		default:
			batch += encoding::SB(address, src, operation_displacement);
			break;
	}
}

void codegen::Instruction::handle_segment_override(codegen::CodeBatch& batch, encoding::RiscVRegister& base,
												   encoding::RiscVRegister temp) const {
	if (const auto offset = get_segment() == Register::fs ? dispatcher::ExecutionContext::fs_offset
			: get_segment() == Register::gs ? dispatcher::ExecutionContext::gs_offset : 0)
	{
		batch += encoding::LD(temp, helper::context_address, offset);

		if (base != RiscVRegister::zero)
			batch += encoding::ADD(temp, base, temp);

		base = temp;
	}
}

void codegen::Instruction::evaluate_zero(CodeBatch& batch) const {
	batch += encoding::LD(RiscVRegister::t4, context_address, flags::Info::zero_value_offset);
	batch += encoding::SEQZ(RiscVRegister::t4, RiscVRegister::t4);
}

void codegen::Instruction::evaluate_sign(CodeBatch& batch, encoding::RiscVRegister temp) const {
	// load the shift amount
	batch += encoding::LBU(RiscVRegister::t4, context_address, flags::Info::sign_size_offset);

	// load the value
	batch += encoding::LD(temp, context_address, flags::Info::sign_value_offset);

	// shift the value
	batch += encoding::SRL(temp, temp, RiscVRegister::t4);
	batch += encoding::ANDI(RiscVRegister::t4, temp, 1);
}

void codegen::Instruction::evaluate_parity(CodeBatch& batch, encoding::RiscVRegister temp) const {
	// load the pf_value
	batch += encoding::LBU(temp, helper::context_address, flags::Info::parity_value_offset);

	// calculate the pf
	batch += encoding::SRLI(RiscVRegister::t4, temp, 4);
	batch += encoding::XOR(temp, temp, RiscVRegister::t4);
	batch += encoding::SRLI(RiscVRegister::t4, temp, 2);
	batch += encoding::XOR(temp, temp, RiscVRegister::t4);
	batch += encoding::SRLI(RiscVRegister::t4, temp, 1);
	batch += encoding::XOR(temp, temp, RiscVRegister::t4);

	// only look at the least significant bit
	batch += encoding::ANDI(temp, temp, 1);

	// set if the parity flag is set (bit is 0)
	batch += encoding::SEQZ(RiscVRegister::t4, temp);
}

void codegen::Instruction::evaluate_overflow(CodeBatch& batch) const {
	// load the jump table offset
	batch += LHU(RiscVRegister::t4, helper::context_address, flags::Info::overflow_operation_offset);

	// jump into the jump table
	jump_table::jump_table_offset(batch, RiscVRegister::t4);
}

void codegen::Instruction::evaluate_carry(CodeBatch& batch) const {
	// load the jump table offset
	batch += LHU(RiscVRegister::t4, helper::context_address, flags::Info::carry_operation_offset);

	// jump into the jump table
	jump_table::jump_table_offset(batch, RiscVRegister::t4);
}

void codegen::Instruction::update_zero(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the zero-flag
	if ((update_flags & flags::zero) == 0)
		return;

	if (set) {
		batch += encoding::SD(helper::context_address, RiscVRegister::zero, flags::Info::zero_value_offset);
	} else {
		batch += encoding::ADDI(temp, encoding::RiscVRegister::zero, 1);
		batch += encoding::SD(helper::context_address, temp, flags::Info::zero_value_offset);
	}
}

void codegen::Instruction::update_zero(CodeBatch& batch, encoding::RiscVRegister va, uint8_t size) const {
	// check if the instruction has to update the zero-flag
	if ((update_flags & flags::zero) == 0)
		return;

	// if the register size is 8 then we don't need to store 0 into the memory operand
	if (size == 8) {
		batch += encoding::SD(helper::context_address, va, flags::Info::zero_value_offset);
		return;
	}

	// clear the zf_value because the upper bits may still be set
	batch += SD(helper::context_address, RiscVRegister::zero, flags::Info::zero_value_offset);

	switch (size) {
		case 1:
			batch += SB(helper::context_address, va, flags::Info::zero_value_offset);
			break;

		case 2:
			batch += SH(helper::context_address, va, flags::Info::zero_value_offset);
			break;

		case 4:
			batch += SW(helper::context_address, va, flags::Info::zero_value_offset);
			break;

		default:
			throw std::runtime_error("Invalid register size.");
	}

}

void codegen::Instruction::update_sign(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the sign-flag
	if ((update_flags & flags::sign) == 0)
		return;

	if (set) {
		batch += encoding::ADDI(temp, encoding::RiscVRegister::zero, -1);
		batch += encoding::SD(helper::context_address, temp, flags::Info::sign_value_offset);
	} else {
		batch += encoding::SD(helper::context_address, RiscVRegister::zero, flags::Info::sign_value_offset);
	}
}

void codegen::Instruction::update_sign(CodeBatch& batch, encoding::RiscVRegister va, uint8_t size,
									   encoding::RiscVRegister temp) const {
	// check if the instruction has to update the sign-flag
	if ((update_flags & flags::sign) == 0)
		return;

	// store the sf_value
	batch += encoding::SD(helper::context_address, va, flags::Info::sign_value_offset);

	// store the size in bits - 1 (the evaluate function shifts by that amount to get the value of the carry flag)
	batch += encoding::ADDI(temp, RiscVRegister::zero, size * 8 - 1);
	batch += encoding::SB(helper::context_address, temp, flags::Info::sign_size_offset);

}

void codegen::Instruction::update_parity(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the parity-flag
	if ((update_flags & flags::parity) == 0)
		return;

	if (set) {
		batch += encoding::SB(helper::context_address, RiscVRegister::zero, flags::Info::parity_value_offset);
	} else {
		batch += encoding::ADDI(temp, RiscVRegister::zero, 1);
		batch += encoding::SB(helper::context_address, temp, flags::Info::parity_value_offset);
	}
}

void codegen::Instruction::update_parity(CodeBatch& batch, encoding::RiscVRegister va) const {
	// check if the instruction has to update the parity-flag
	if ((update_flags & flags::parity) == 0)
		return;

	batch += encoding::SB(helper::context_address, va, flags::Info::parity_value_offset);
}

void codegen::Instruction::update_overflow(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & flags::overflow) == 0)
		return;

	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(
															   set ? jump_table::Entry::overflow_set
																   : jump_table::Entry::overflow_clear) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::overflow_operation_offset);
}

void codegen::Instruction::update_overflow(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va,
										   encoding::RiscVRegister vb, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & flags::overflow) == 0)
		return;

	// store the values
	batch += encoding::SD(helper::context_address, va, flags::Info::overflow_values_offset);
	batch += encoding::SD(helper::context_address, vb, flags::Info::overflow_values_offset + 8);

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(entry) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::overflow_operation_offset);
}

void codegen::Instruction::update_overflow_single(codegen::CodeBatch& batch, codegen::jump_table::Entry entry,
												  encoding::RiscVRegister va, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & flags::overflow) == 0)
		return;

	batch += encoding::SD(helper::context_address, va, flags::Info::overflow_values_offset);

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(entry) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::overflow_operation_offset);
}

void codegen::Instruction::update_overflow_single(codegen::CodeBatch& batch, encoding::RiscVRegister vb) const {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & flags::overflow) == 0)
		return;

	// store the values
	batch += encoding::SD(helper::context_address, vb, flags::Info::overflow_values_offset + 8);
}

void codegen::Instruction::update_overflow(codegen::CodeBatch& batch, encoding::RiscVRegister entry, encoding::RiscVRegister va,
										   encoding::RiscVRegister vb) const {
	// store the values
	batch += encoding::SD(helper::context_address, va, flags::Info::overflow_values_offset);
	batch += encoding::SD(helper::context_address, vb, flags::Info::overflow_values_offset + 8);

	// store the jump table index
	batch += encoding::SH(helper::context_address, entry, flags::Info::overflow_operation_offset);
}

void codegen::Instruction::update_carry(CodeBatch& batch, bool set, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the carry-flag
	if ((update_flags & flags::carry) == 0)
		return;

	batch += encoding::ADDI(temp, RiscVRegister::zero,
							static_cast<uint16_t>(set ? jump_table::Entry::carry_set : jump_table::Entry::carry_clear) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::carry_operation_offset);
}

void codegen::Instruction::update_carry(CodeBatch& batch, jump_table::Entry entry, encoding::RiscVRegister va,
										encoding::RiscVRegister vb, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the carry-flag
	if ((update_flags & flags::carry) == 0)
		return;

	// store the values
	batch += encoding::SD(helper::context_address, va, flags::Info::carry_values_offset);
	batch += encoding::SD(helper::context_address, vb, flags::Info::carry_values_offset + 8);

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(entry) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::carry_operation_offset);
}

void codegen::Instruction::update_carry_single(codegen::CodeBatch& batch, codegen::jump_table::Entry entry,
											   encoding::RiscVRegister va, encoding::RiscVRegister temp) const {
	// check if the instruction has to update the carry-flag
	if ((update_flags & flags::carry) == 0)
		return;

	batch += encoding::SD(helper::context_address, va, flags::Info::carry_values_offset);

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(entry) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::carry_operation_offset);
}

void codegen::Instruction::update_carry_single(codegen::CodeBatch& batch, encoding::RiscVRegister vb) const {
	// check if the instruction has to update the carry-flag
	if ((update_flags & flags::carry) == 0)
		return;

	// store the values
	batch += encoding::SD(helper::context_address, vb, flags::Info::carry_values_offset + 8);
}

void codegen::Instruction::update_carry(codegen::CodeBatch& batch, encoding::RiscVRegister entry, encoding::RiscVRegister va,
										encoding::RiscVRegister vb) const {
	// store the values
	batch += encoding::SD(helper::context_address, va, flags::Info::carry_values_offset);
	batch += encoding::SD(helper::context_address, vb, flags::Info::carry_values_offset + 8);

	// store the jump table index
	batch += encoding::SH(helper::context_address, entry, flags::Info::carry_operation_offset);
}

void codegen::Instruction::update_carry_unsupported(CodeBatch& batch, const char* instruction,
													encoding::RiscVRegister temp) const {
	// check if the instruction has to update the carry-flag
	if ((update_flags & flags::carry) == 0)
		return;

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(jump_table::Entry::unsupported_carry) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::carry_operation_offset);

	// store the pointer
	helper::load_immediate(batch, reinterpret_cast<uintptr_t>(instruction), temp);
	batch += encoding::SD(helper::context_address, temp, flags::Info::carry_ptr_offset);
}

void codegen::Instruction::update_carry_high_level(CodeBatch& batch, c_callback_t callback,
												   encoding::RiscVRegister temp) const {
	// check if the instruction has to update the carry-flag
	if ((update_flags & flags::carry) == 0)
		return;

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(jump_table::Entry::high_level_carry) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::carry_operation_offset);

	// store the pointer
	helper::load_immediate(batch, reinterpret_cast<uintptr_t>(callback), temp);
	batch += encoding::SD(helper::context_address, temp, flags::Info::carry_ptr_offset);
}

void codegen::Instruction::update_overflow_unsupported(CodeBatch& batch, const char* instruction,
													   encoding::RiscVRegister temp) const {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & flags::overflow) == 0)
		return;

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(jump_table::Entry::unsupported_overflow) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::overflow_operation_offset);

	// store the pointer
	helper::load_immediate(batch, reinterpret_cast<uintptr_t>(instruction), temp);
	batch += encoding::SD(helper::context_address, temp, flags::Info::overflow_ptr_offset);
}

void codegen::Instruction::update_overflow_high_level(CodeBatch& batch, c_callback_t callback,
													  encoding::RiscVRegister temp) const {
	// check if the instruction has to update the overflow-flag
	if ((update_flags & flags::overflow) == 0)
		return;

	// store the jump table index
	batch += encoding::ADDI(temp, RiscVRegister::zero, static_cast<uint16_t>(jump_table::Entry::high_level_overflow) * 4);
	batch += encoding::SH(helper::context_address, temp, flags::Info::overflow_operation_offset);

	// store the pointer
	helper::load_immediate(batch, reinterpret_cast<uintptr_t>(callback), temp);
	batch += encoding::SD(helper::context_address, temp, flags::Info::overflow_ptr_offset);
}

void codegen::Instruction::call_high_level(CodeBatch& batch, c_callback_t callback) const {
	// load the address into t4
	load_immediate(batch, reinterpret_cast<uintptr_t>(callback), encoding::RiscVRegister::t4);

	// generate the code to invoke the jump-table-entry
	jump_table::jump_table_entry(batch, jump_table::Entry::c_wrapper);
}