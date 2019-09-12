#include "oxtra/dispatcher/dispatcher.h"
#include "oxtra/dispatcher/syscall_map.h"
#include "execution_context.h"
#include "debugger/debugger.h"
#include <spdlog/spdlog.h>
#include <elf.h>

using namespace dispatcher;
using namespace codegen;
using namespace utils;

/* This statement is required and must not be removed.
 * Otherwise reroute_static & reroute_dynamic will not compile,
 * because they use this function with this prototype, but they
 * are not capable of triggering the compiler to compile this function.
 * Thus we have to trigger if from outside. */
template void spdlog::info(const char*, const unsigned long&);

Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args, char** envp)
		: _elf(elf), _args(args), _envp(envp), _codegen(args, elf) {}

long Dispatcher::run() {
	init_guest_context();

	// initialize the debugger if necessary
	std::unique_ptr<debugger::Debugger> debugger = nullptr;
	if (_args.get_debugging() > 0) {
		debugger = std::make_unique<debugger::Debugger>(_elf, _args.get_debugging() == 2,
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

	char** envp = _envp;
	Elf64_auxv_t* auxvs;
	size_t env_count, auxv_count;

	// count environment pointers and search for aux vectors
	while (*envp++ != nullptr); // find the first entry after the null entry
	env_count = envp - _envp; // size includes nullptr

	// aux vectors begin after the envp (null separated)
	auxvs = reinterpret_cast<Elf64_auxv_t*>(envp);

	// count aux vectors
	auto auxv = auxvs;
	while ((auxv++)->a_type != AT_NULL);
	auxv_count = auxv - auxvs; // size includes nullptr

	// the count of the necessary arguments with zero entries in between (in bytes). Zero paddings are included in size
	size_t min_stack_size = auxv_count * sizeof(Elf64_auxv_t) + // auxv
							env_count * sizeof(char*) + // envp
							sizeof(char*) + _args.get_guest_arguments().size() * sizeof(char*) + // argv
							sizeof(size_t); // argc

	// initialize the stack
	_context.guest.map.rsp = reinterpret_cast<uintptr_t>(new uint8_t[_args.get_stack_size()]) + _args.get_stack_size();
	_context.guest.map.rbp = 0; // unspecified

	// guaranteed to be 16-byte aligned, meaning that the lower 4 bits have to be cleared
	_context.guest.map.rsp = (_context.guest.map.rsp - min_stack_size) & ~(static_cast<uintptr_t>(0xfu));

	auto rsp = reinterpret_cast<size_t*>(_context.guest.map.rsp);

	//TODO: reverse argument order???

	// put argc and argv on stack
	*(rsp++) = _args.get_guest_arguments().size();
	for (size_t i = 0; i < _args.get_guest_arguments().size(); i++) {
		*(rsp++) = (size_t)_args.get_guest_arguments()[i].c_str();
	}
	*(rsp++) = 0;

	// put environment strings on the stack
	memcpy(rsp, _envp, env_count * sizeof(char*));
	rsp += env_count;

	// put aux vectors on the stack
	memcpy(rsp, auxvs, auxv_count * sizeof(Elf64_auxv_t));

	/*rsp = reinterpret_cast<size_t*>(_context.guest.map.rsp);
	printf("stack: 0x%p\n", rsp);
	printf("argc: %d\n", rsp[0]);
	printf("argv: %s\n", rsp[1]);
	printf("argv: %s\n", rsp[2]);
	printf("arg delimiter: %x\n", rsp[3]);

	for (int i = 0; i < env_count; i++) {
		printf("environment: %s\n", rsp[3 + i + 1]);
	}

	rsp = rsp + 3 + 1 + env_count;
	auto aux_rsp = (Elf64_auxv_t*)(rsp);
	for (int i = 0; i < auxv_count; i++) {
		printf("A_Type %ld is: 0x%x\n", aux_rsp->a_type, aux_rsp->a_un.a_val);
		aux_rsp++;
	}
	 */
}

long Dispatcher::virtualize_syscall(const ExecutionContext* context) {
	using namespace internal;

	// the x86 syscall index
	const auto guest_index = context->guest.map.rax;

	// we emulate exit
	if (guest_index == 60) {
		guest_exit(context->guest.map.rdi);
		return -1;
	}

	/**
	 * if the syscall is handled write the return value to _guest_context.a0.
	 */

	// is this an invalid index?
	if (guest_index < syscall_map.size()) {
		const auto syscall_index = syscall_map[guest_index];

		// print the systemcall with its attributes
		spdlog::info("syscall: [{:03}]->[{:03}]({:#x}, {:#x}, {:#x}, {:#x}, {:#x}, {:#x})", guest_index, syscall_index,
					 context->guest.map.rdi, context->guest.map.rsi, context->guest.map.rdx,
					 context->guest.map.r10, context->guest.map.r8, context->guest.map.r9);
		if (syscall_index >= 0)
			return syscall_index;
	}

	// we will only reach this if the system call is not supported
	char fault_message[256];
	snprintf(fault_message, sizeof(fault_message), "Guest tried to call an unsupported syscall (%li).", guest_index);
	dispatcher::Dispatcher::fault_exit(fault_message);
	return -1;
}
