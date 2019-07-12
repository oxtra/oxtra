#include "oxtra/dispatcher/dispatcher.h"
#include <spdlog/spdlog.h>

using namespace dispatcher;
using namespace codegen;
using namespace utils;

Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args)
		: _elf(elf), _args(args), _codegen(args, elf) {
}

long Dispatcher::run() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

	//TODO: add argument for stack-size (default: 0x3200000)
	//TODO: initialize registers (ABI-conform)
	//TODO: initialize stack (ABI-Conform)

	register uintptr_t gp_reg asm("gp");
	register uintptr_t tp_reg asm("tp");

	// initialize guest-context
	_guest_context.gp = gp_reg;
	_guest_context.tp = tp_reg;
	_guest_context.sp = reinterpret_cast<uintptr_t>(new uint8_t[0x3200000]) + 0x3200000;
	_guest_context.fp = _guest_context.sp;
	_guest_context.s8 = reinterpret_cast<uintptr_t>(Dispatcher::reroute_static);
	_guest_context.s9 = reinterpret_cast<uintptr_t>(Dispatcher::reroute_dynamic);
	_guest_context.s11 = reinterpret_cast<uintptr_t>(&_guest_context);

	// switch the context and begin translation
	const char* error_string = nullptr;
	const auto exit_code = guest_enter(&_guest_context, _elf.get_entry_point(), &error_string);

	// check if the guest has ended with and error
	if (error_string != nullptr)
		throw std::runtime_error(error_string);
	return exit_code;

#pragma GCC diagnostic pop
}