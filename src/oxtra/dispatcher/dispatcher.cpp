#include "oxtra/dispatcher/dispatcher.h"
#include "oxtra/dispatcher/syscalls.h"
#include "execution_context.h"
#include "oxtra/debugger/debugger.h"
#include "oxtra/logger/logger.h"
#include "elf.h"

#include <cstring>

using namespace dispatcher;
using namespace codegen;
using namespace utils;

Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args, char** envp)
		: _elf(elf), _args(args), _envp(envp), _codegen(args, elf) {}

long Dispatcher::run() {
	init_guest_context();

	// initialize the debugger if necessary
	std::unique_ptr<debugger::Debugger> debugger = nullptr;
	if (_args.get_debugging() > 0) {
		debugger = std::make_unique<debugger::Debugger>(_elf, &_context, _args.get_debugging() == 2,
														_context.guest.map.rsp - _args.get_stack_size(),
														_args.get_stack_size());
		_context.debugger = debugger.get();
	}

	// set the flags indirectly
	_context.flag_info.overflow_operation = static_cast<uint16_t>(jump_table::Entry::overflow_clear) * 4;
	_context.flag_info.carry_operation = static_cast<uint16_t>(jump_table::Entry::carry_clear) * 4;
	_context.flag_info.zero_value = 1;
	_context.flag_info.sign_size = 0;
	_context.flag_info.parity_value = 1;
	_context.flag_info.carry_pointer = 0;
	_context.flag_info.overflow_pointer = 0;

	// switch the context and begin translation
	const char* error_string = nullptr;
	const auto exit_code = guest_enter(&_context, _elf.get_entry_point(), &error_string);

	// check if the guest has ended with and error
	if (error_string != nullptr)
		throw std::runtime_error(error_string);
	return exit_code;
}

void Dispatcher::init_guest_context() {
	// https://software.intel.com/sites/default/files/article/402129/mpx-linux64-abi.pdf

	register uintptr_t gp_reg asm("gp");
	register uintptr_t tp_reg asm("tp");

	_context.guest.gp = gp_reg;
	_context.guest.tp = tp_reg;

	_context.guest.map.rdx = 0; // function-pointer to on-exit
	_context.guest.map.jump_table = reinterpret_cast<uintptr_t>(jump_table::table_address);
	_context.guest.map.context = reinterpret_cast<uintptr_t>(&_context);
	_context.codegen = &_codegen;
	_context.program_break = _context.initial_break = _context.last_break_page = _elf.get_base_vaddr() + _elf.get_image_size();

	auto envp = _envp;

	// count environment pointers and search for aux vectors
	while (*envp++ != nullptr); // find the first entry after the null entry
	const size_t env_count = envp - _envp; // size includes nullptr

	// aux vectors begin after the envp (null separated)
	const auto auxvs = reinterpret_cast<Elf64_auxv_t*>(envp);

	// count aux vectors
	auto auxv = auxvs;
	while ((auxv++)->a_type != AT_NULL);
	const size_t
			auxv_count = auxv - auxvs, // size includes null entry
			auxv_size = auxv_count * sizeof(Elf64_auxv_t) - 8; // last entry is 8 bytes

	// the count of the necessary arguments with zero entries in between (in bytes). Zero paddings are included in size
	const auto min_stack_size = utils::page_align(auxv_size + // auxv
						  env_count * sizeof(char*) + // envp
						  sizeof(char*) + _args.get_guest_arguments().size() * sizeof(char*) + // argv
						  sizeof(size_t)); // argc

	// initialize the stack (assume a page for the arg, env pointers and aux vectors)
	const auto stack_size = utils::page_align(_args.get_stack_size());

	const auto stack_memory = reinterpret_cast<uintptr_t>(mmap(nullptr, stack_size, PROT_READ | PROT_WRITE,
								   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));

	// page aligned by only using page-aligned values
	_context.guest.map.rsp = stack_memory + stack_size - min_stack_size;
	_context.guest.map.rbp = 0; // unspecified

	auto rsp = reinterpret_cast<size_t*>(_context.guest.map.rsp);

	// put argc and argv on stack
	*rsp++ = _args.get_guest_arguments().size() + 1;
	*rsp++ = reinterpret_cast<size_t>(_args.get_guest_path());
	for (const auto& arg : _args.get_guest_arguments()) {
		*rsp++ = reinterpret_cast<size_t>(arg.c_str());
	}
	*rsp++ = 0;

	// put environment strings on the stack
	std::memcpy(rsp, _envp, env_count * sizeof(char*));
	rsp += env_count;

	// put aux vectors on the stack
	std::memcpy(rsp, auxvs, auxv_size);

	const auto guest_auxv = reinterpret_cast<Elf64_auxv_t*>(rsp);
	for (auto entry = guest_auxv; entry->a_type != AT_NULL; ++entry) {
		if (entry->a_type == AT_PHDR) {
			const auto elf_hdr = reinterpret_cast<const Elf64_Ehdr*>(_elf.get_base_vaddr());
			entry->a_un.a_val = _elf.get_base_vaddr() + elf_hdr->e_phoff;
		} else if (entry->a_type == AT_ENTRY) {
			entry->a_un.a_val = _elf.get_entry_point();
		}
	}
}

long Dispatcher::virtualize_syscall(ExecutionContext* context) {
	using namespace syscalls;

	// the x86 syscall index
	const auto guest_index = context->guest.map.rax;

	// is this an invalid index?
	if (guest_index < syscall_map.size()) {

		// get the entry from the syscall map
		const auto entry = syscall_map[guest_index];
		if (entry.is_valid()) {

			// do we forward this syscall to the riscv kernel?
			if (entry.is_forwarded()) {
				// print the systemcall with its attributes
				logger::syscall("syscall: [{:03}]->[{:03}]({:#x}, {:#x}, {:#x}, {:#x}, {:#x}, {:#x})\n",
								guest_index, entry.riscv_index,
								context->guest.map.rdi, context->guest.map.rsi, context->guest.map.rdx,
								context->guest.map.r10, context->guest.map.r8, context->guest.map.r9);

				return entry.riscv_index;
			}

				// should this syscall be emulated?
			else /*if (entry.is_emulated())*/ {
				entry.emulation_fn(context);
				return -1;
			}
		}
	}

	// we will only reach this if the system call is not supported
	char fault_message[256];
	snprintf(fault_message, sizeof(fault_message), "Guest tried to call an unsupported syscall (%li).", guest_index);
	dispatcher::Dispatcher::fault_exit(fault_message);
	return -1;
}