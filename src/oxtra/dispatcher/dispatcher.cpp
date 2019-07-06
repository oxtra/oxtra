#include "oxtra/dispatcher/dispatcher.h"
#include <spdlog/spdlog.h>

using namespace dispatcher;
using namespace codegen;
using namespace utils;

extern "C" int guest_enter(Context* context, utils::host_addr_t entry);

Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args)
		: _elf(elf), _args(args), _codegen(args, elf) {
}

int Dispatcher::run() {
	static_assert(CodeGenerator::context_address == encoding::RiscVRegister::s11,
				  "dispatcher::run requires s11");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

	//TODO: add argument for stack-size (default: 0x3200000)
	//TODO: initialize registers (ABI-conform)
	//TODO: initialize stack (ABI-Conform)

	register uintptr_t gp_reg asm("gp");
	register uintptr_t tp_reg asm("tp");
	register uintptr_t sp_reg asm("sp");

	// initialize guest-context
	_guest_context.gp = gp_reg;
	_guest_context.tp = tp_reg;
	_guest_context.sp = reinterpret_cast<uintptr_t>(new uint8_t[0x3200000]) + 0x3200000;
	_guest_context.fp = sp_reg;
	_guest_context.s8 = reinterpret_cast<uintptr_t>(Dispatcher::reroute_static);
	_guest_context.s9 = reinterpret_cast<uintptr_t>(Dispatcher::reroute_dynamic);
	_guest_context.s11 = reinterpret_cast<uintptr_t>(&_guest_context);

	// translate the first basic block and execute it
	const auto init_address = _codegen.translate(_elf.get_entry_point());

	// translate the first basic block and execute it
	return guest_enter(&_guest_context, init_address);

#pragma GCC diagnostic pop
}

void Dispatcher::reroute_static() {
	static_assert(CodeGenerator::address_destination == encoding::RiscVRegister::t3,
				  "dispatcher::reroute_static requires t3");
	static_assert(CodeGenerator::context_address == encoding::RiscVRegister::s11,
				  "dispatcher::reroute_static requires s11");

	// capture the guest context
	capture_context_s11
	{
		printf("reroute_static!\n");

		// extract dispatcher
		register uintptr_t s11_register asm("s11");
		const auto _this = reinterpret_cast<Dispatcher*>(s11_register);

		// translate address
		const auto translated_address = _this->_codegen.translate(_this->_guest_context.t3);

		// write new absolute address
		_this->_codegen.update_basic_block_address(_this->_guest_context.ra, translated_address);
		_this->_guest_context.t3 = translated_address;
	}
	restore_context_s11
}

void Dispatcher::reroute_dynamic() {
	static_assert(CodeGenerator::address_destination == encoding::RiscVRegister::t3,
				  "dispatcher::reroute_dynamic requires t3");
	static_assert(CodeGenerator::context_address == encoding::RiscVRegister::s11,
				  "dispatcher::reroute_dynamic requires s11");

	// capture the guest context
	capture_context_s11
	{
		printf("reroute_dynamic!\n");

		// extract dispatcher
		register uintptr_t s11_register asm("s11");
		const auto _this = reinterpret_cast<Dispatcher*>(s11_register);

		// translate address
		_this->_guest_context.t3 = _this->_codegen.translate(_this->_guest_context.t3);
	}
	restore_context_s11
}