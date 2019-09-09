#include "debugger.h"
#include <spdlog/spdlog.h>
#include <sstream>
#include <string>
#include <iostream>

#include "oxtra/dispatcher/dispatcher.h"


//TODO: add stack-print


void debugger::DebuggerBatch::print() const {
	for (size_t i = 1; i < count; ++i)
		spdlog::trace("    [{:02}] = {}", i - 1, decoding::parse_riscv(riscv[i]));
}

debugger::Debugger* debugger::Debugger::active_debugger = nullptr;

debugger::Debugger::Debugger(const elf::Elf& elf) : _elf{elf} {
	// initialize the attributes
	_halt = true;
	_bp_count = 0;
	_state = DebugState::none;
	_bp_counter = 0;
	_current = nullptr;

	// mark this as the active debugger
	active_debugger = this;
}

debugger::Debugger::~Debugger() {
	// reset the active debugger
	if (active_debugger == this)
		active_debugger = nullptr;
}

void debugger::Debugger::begin_block(codegen::CodeBatch& batch) {
	unused_parameter(batch);
}

void debugger::Debugger::insert(codegen::CodeBatch& batch, codegen::Instruction* inst) {
	unused_parameter(inst);

	// check if a debugger exists
	if (!active_debugger)
		return;

	// add the instruction to jump to the debugger
	codegen::jump_table::jump_debugger(batch);
}

void debugger::Debugger::end_block(codegen::CodeBatch& batch, codegen::codestore::BlockEntry* block) {
	unused_parameter(batch);

	// check if a debugger exists
	if (!active_debugger)
		return;

	// compute the end of the riscv-instructions
	utils::host_addr_t riscv_end = block->riscv_start;
	for (size_t i = 0; i < block->instruction_count; i++)
		riscv_end += block->offsets[i].riscv;

	// extract the current-current_block
	size_t current_index = active_debugger->_blocks.size() + 1;
	if (active_debugger->_current != nullptr) {
		for (size_t i = 0; i < active_debugger->_blocks.size(); i++) {
			if (&active_debugger->_blocks[i] == active_debugger->_current) {
				current_index = i;
				break;
			}
		}
	}

	// add the block to the list
	active_debugger->_blocks.emplace_back(block, riscv_end);

	// update the current block
	if (current_index != active_debugger->_blocks.size())
		active_debugger->_current = &active_debugger->_blocks[current_index];

	// update the break-points
	active_debugger->update_break_points(active_debugger->_blocks.back());
}

void debugger::Debugger::entry(dispatcher::ExecutionContext* context, uintptr_t break_point) {
	// check if the debugger should be entered
	utils::guest_addr_t address = enter_break(break_point, context->guest.ra);
	if (address == 0)
		return;

	// enter the loop
	std::string menu_string;
	do {
		// print the separating line and the current address
		std::stringstream out;
		out << '\n' << std::string(100, '-') << '\n';
		out << "guest-address: " << print_number(address, true) << '\n';
		out << "host-address : " << print_number(context->guest.ra, true) << "\n\n";

		// print the break-points
		if (_state & DebugState::print_bp)
			out << print_break_points() << '\n';

		// print the flags
		if (_state & DebugState::print_flags)
			out << print_flags(context) << '\n';

		// print the registers
		if (_state & DebugState::print_reg)
			out << print_reg(context, (_state & DebugState::reg_dec) == 0, _state & DebugState::reg_riscv) << '\n';

		// print the assembly
		if (_state & DebugState::print_asm)
			out << print_assembly(address, _current) << '\n';

		// check if the menu has to be printed
		if (menu_string.empty()) {
			// print the reason for the break
			if (break_point != halt_break)
				out << "break-point hit! (index: " << std::dec << break_point << ")\n";
			if ((_state & DebugState::init) == 0) {
				out << "initial halt! Type \"help\" for a help-menu.\n";
				_state |= DebugState::init;
			}
			if (_bp_counter == 0 && (_state & DebugState::await_counter))
				out << "run-counter ellapsed.\n";
			if (_state & DebugState::await_step)
				out << "stepped by one instruction!\n";
			if (address == _current->entry->x86_start && (_state & DebugState::await_sblock))
				out << "beginning of basic-block reached!\n";
			else if (address == _current->entry->x86_end - _current->entry->offsets[_current->entry->instruction_count - 1].x86
					 && (_state & DebugState::await_eblock))
				out << "ending of basic-block reached!\n";

			// clear the await-details and the break-point
			_state &= ~DebugState::await;
			break_point = halt_break;
		} else
			out << menu_string << '\n';

		// print the screen
		std::cout << out.str() << std::endl;

		// handle input
		menu_string = parse_input(address, context);
	} while (!menu_string.empty());

	// print the closing line
	std::cout << '\n' << std::string(100, '-') << std::endl;

	// update the halt-flag
	_halt = (_state & DebugState::await) > 0;
}

utils::guest_addr_t debugger::Debugger::resolve_block(utils::host_addr_t address) {
	// check if a current basic block exists
	if (_current != nullptr) {
		if (address < _current->entry->riscv_start || address >= _current->riscv_end)
			_current = nullptr;
	}

	// resolve the new block, if necessary
	if (_current == nullptr) {
		// look for the basic-block
		for (auto& block : _blocks) {
			if (address >= block.entry->riscv_start && address < block.riscv_end) {
				_current = &block;
				break;
			}
		}
		if (_current == nullptr)
			return 0;
	}

	// resolve the x86-address
	utils::host_addr_t riscv_counter = _current->entry->riscv_start;
	utils::guest_addr_t x86_counter = _current->entry->x86_start;
	for (size_t i = 0; i < _current->entry->instruction_count; i++) {
		if (riscv_counter == address)
			return x86_counter;
		else if (riscv_counter > address)
			return x86_counter - _current->entry->offsets[i - 1].x86;
		riscv_counter += _current->entry->offsets[i].riscv;
		x86_counter += _current->entry->offsets[i].x86;
	}

	// return the last instruction, as this point can only be reached, if address points within one instruction
	return x86_counter - _current->entry->offsets[_current->entry->instruction_count - 1].x86;
}

utils::guest_addr_t debugger::Debugger::enter_break(uintptr_t break_point, utils::host_addr_t address) {
	// update the counter
	if (_state & DebugState::await_counter) {
		if (_bp_counter > 0)
			_bp_counter--;
		if (_bp_counter == 0)
			return resolve_block(address);
	}

	// check for a stepper
	if (_state & DebugState::await_step)
		return resolve_block(address);

	// check if a break-point has been hit or if this is the init-break
	if (break_point != halt_break || (_state & DebugState::init) == 0)
		return resolve_block(address);

	// check if a basic block has been reached
	if ((_state & DebugState::await_eblock) || (_state & DebugState::await_sblock)) {
		// resolve the address, which should also update the current block
		utils::guest_addr_t guest_addr = resolve_block(address);
		if (guest_addr == 0)
			return 0;

		// check if a beginning or an end has been reached
		if ((guest_addr == _current->entry->x86_start) && (_state & DebugState::await_sblock))
			return guest_addr;
		if ((guest_addr == _current->entry->x86_end - _current->entry->offsets[_current->entry->instruction_count - 1].x86) &&
			(_state & DebugState::await_eblock))
			return guest_addr;
	}
	return 0;
}

void debugger::Debugger::update_break_points(const BlockEntry& block) {
	// iterate through the x86-break-points and check if one of them has been translated
	for (size_t i = 0; i < _bp_count; i++) {
		if (_bp_array[i] != 0)
			continue;

		// check if the address lies within the new block
		if (_bp_x86_array[i] >= block.entry->x86_start && _bp_x86_array[i] < block.entry->x86_end) {
			// initialize the counters (riscv + 4, as the first instruction is the jump to the debug-handler)
			uintptr_t x86_counter = block.entry->x86_start;
			uintptr_t riscv_counter = block.entry->riscv_start + 4;

			// iterate through the instructions and look for the one of the break-point
			for (size_t j = 0; j < block.entry->instruction_count; j++) {
				if (x86_counter == _bp_x86_array[i]) {
					_bp_array[i] = riscv_counter;
					break;
				} else if (x86_counter > _bp_x86_array[i]) {
					_bp_x86_array[i] = x86_counter - block.entry->offsets[j - 1].x86;
					_bp_array[i] = riscv_counter - block.entry->offsets[j - 1].riscv;
					break;
				}
				riscv_counter += block.entry->offsets[j].riscv;
				x86_counter += block.entry->offsets[j].x86;
			}

			// if the break-point has not been set yet, it must lie within the last instruction
			if (_bp_array[i] == 0) {
				_bp_x86_array[i] = block.entry->x86_end - block.entry->offsets[block.entry->instruction_count - 1].x86;
				_bp_array[i] = block.riscv_end - block.entry->offsets[block.entry->instruction_count - 1].riscv;
			}
		}

		// check if a break-point already exists for this address
		if (_bp_array[i] == 0)
			continue;
		bool move = false;
		for (size_t j = 0; j < _bp_count; j++) {
			if (move) {
				_bp_x86_array[j - 1] = _bp_x86_array[j];
				_bp_array[j - 1] = _bp_array[j];
				continue;
			}
			if (i == j || _bp_x86_array[i] != _bp_x86_array[j])
				continue;
			move = true;
		}

		// check if an entry has been removed
		if (move)
			_bp_count--;
	}
}

void debugger::Debugger::translate_break_point(uint16_t index) {
	// iterate through the blocks and check if one of them suits the break-point
	utils::guest_addr_t address = _bp_x86_array[index];
	for (auto& block : _blocks) {
		// check if the address lies within the block
		if (address >= block.entry->x86_start && address < block.entry->x86_end) {
			// initialize the counters (riscv + 4, as the first instruction is the jump to the debug-handler)
			uintptr_t x86_counter = block.entry->x86_start;
			uintptr_t riscv_counter = block.entry->riscv_start + 4;

			// iterate through the instructions and look for the one of the break-point
			for (size_t j = 0; j < block.entry->instruction_count; j++) {
				if (x86_counter == address) {
					_bp_array[index] = riscv_counter;
					return;
				} else if (x86_counter > address) {
					_bp_x86_array[index] = x86_counter - block.entry->offsets[j - 1].x86;
					_bp_array[index] = riscv_counter - block.entry->offsets[j - 1].riscv;
					return;
				}
				riscv_counter += block.entry->offsets[j].riscv;
				x86_counter += block.entry->offsets[j].x86;
			}

			// if the break-point has not been set yet, it must lie within the last instruction
			_bp_x86_array[index] = block.entry->x86_end - block.entry->offsets[block.entry->instruction_count - 1].x86;
			_bp_array[index] = block.riscv_end - block.entry->offsets[block.entry->instruction_count - 1].riscv;
			return;
		}
	}

	// clear the break-point, as the block has not yet been translated
	_bp_array[index] = 0;
}