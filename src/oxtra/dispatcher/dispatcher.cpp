#include "oxtra/dispatcher/dispatcher.h"
#include "oxtra/dispatcher/syscall_map.h"
#include "context.h"
#include <spdlog/spdlog.h>

using namespace dispatcher;
using namespace codegen;
using namespace utils;

/* This statement is required and must not be removed.
 * Otherwise reroute_static & reroute_dynamic will not compile,
 * because they use this function with this prototype, but they
 * are not capable of triggering the compiler to compile this function.
 * Thus we have to trigger if from outside. */
template void spdlog::info(const char*, const unsigned long&);

Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args)
		: _elf(elf), _args(args), _codegen(args, elf) {
}

long Dispatcher::run() {
	//TODO: add argument for stack-size (default: 0x3200000)
	//TODO: initialize registers (ABI-conform)
	//TODO: initialize stack (ABI-Conform)

	register uintptr_t gp_reg asm("gp");
	register uintptr_t tp_reg asm("tp");

	// initialize guest-context
	_guest_context.gp = gp_reg;
	_guest_context.tp = tp_reg;
	_guest_context.map.rsp = reinterpret_cast<uintptr_t>(new uint8_t[0x3200000]) + 0x3200000;
	_guest_context.map.rbp = _guest_context.map.rsp;
	_guest_context.map.reroute_static = reinterpret_cast<uintptr_t>(Dispatcher::reroute_static);
	_guest_context.map.reroute_dynamic = reinterpret_cast<uintptr_t>(Dispatcher::reroute_dynamic);
	_guest_context.map.syscall_handler = reinterpret_cast<uintptr_t>(Dispatcher::syscall_handler);
	_guest_context.map.context = reinterpret_cast<uintptr_t>(&_guest_context);

	// switch the context and begin translation
	const char* error_string = nullptr;
	const auto exit_code = guest_enter(&_guest_context, _elf.get_entry_point(), &error_string);

	// check if the guest has ended with and error
	if (error_string != nullptr)
		throw std::runtime_error(error_string);
	return exit_code;
}

long Dispatcher::virtualize_syscall() {
	using namespace internal;

	// the x86 syscall index
	const auto guest_index = _guest_context.map.rax;

	// we emulate exit
	if (guest_index == 60) {
		guest_exit(_guest_context.map.rdi);
		return -1;
	}

	/**
	 * if the syscall is handled write the return value to _guest_context.a0.
	 */

	// is this an invalid index?
	if (guest_index < syscall_map.size()) {
		const auto syscall_index = syscall_map[guest_index];

#ifdef DEBUG
		// print the systemcall with its attributes
		spdlog::info("syscall: {}({}, {}, {}, {}, {}, {})", syscall_index,
					 _guest_context.map.rdi, _guest_context.map.rsi, _guest_context.map.rdx,
					 _guest_context.map.r10, _guest_context.map.r8, _guest_context.map.r9);
#endif

		if (syscall_index >= 0)
			return syscall_index;
	}

	// we will only reach this if the system call is not supported
	char fault_message[256];
	snprintf(fault_message, sizeof(fault_message), "Guest tried to call an unsupported syscall (%li).", guest_index);
	fault_exit(fault_message);
	return -1;
}