#include "oxtra/dispatcher/dispatcher.h"
#include <spdlog/spdlog.h>

using namespace dispatcher;
using namespace codegen;
using namespace utils;


Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args)
		: _elf(elf), _args(args), _codegen(args, elf) {
}

int Dispatcher::run() {
	static_assert(CodeGenerator::context_address == encoding::RiscVRegister::s11,
				  "dispatcher::run requires s11");

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

	// capture the host-context
	register uintptr_t s11_register asm("s11") = reinterpret_cast<uintptr_t>(&_host_context);
	capture_context_s11;

	//TODO: add argument for stack-size (default: 0x3200000)
	//TODO: initialize registers (ABI-conform)
	//TODO: initialize stack (ABI-Conform)

	// initialize guest-context
	_guest_context.gp = _host_context.gp;
	_guest_context.tp = _host_context.tp;
	_guest_context.sp = reinterpret_cast<uintptr_t>(new uint8_t[0x3200000]) + 0x3200000;
	_guest_context.fp = _guest_context.sp;
	_guest_context.s8 = reinterpret_cast<uintptr_t>(Dispatcher::host_call);
	_guest_context.s9 = reinterpret_cast<uintptr_t>(Dispatcher::inline_translate);
	_guest_context.s11 = reinterpret_cast<uintptr_t>(&_guest_context);

	// load the new context
	s11_register = reinterpret_cast<uintptr_t>(&_guest_context);
	restore_context_s11;

	// translate the first basic block and execute it
	const auto _this = reinterpret_cast<Dispatcher*>((s11_register - offsetof(Dispatcher, _guest_context)));
	register uintptr_t init_address asm("t0") = _this->_codegen.translate(_this->_elf.get_entry_point());

	// translate the first basic block and execute it
	asm("JALR zero, t0, 0");
	throw std::runtime_error("This should not be reachable!");

#pragma GCC diagnostic pop
}

void Dispatcher::host_call() {
	static_assert(CodeGenerator::address_destination == encoding::RiscVRegister::t3,
				  "dispatcher::host_call requires t3");
	static_assert(CodeGenerator::context_address == encoding::RiscVRegister::s11,
				  "dispatcher::host_call requires s11");

	// capture the guest context
	capture_context_s11;

	// extract dispatcher
	register uintptr_t s11_register asm("s11");
	const auto _this = reinterpret_cast<Dispatcher*>((s11_register - offsetof(Dispatcher, _guest_context)));

	// translate address
	utils::host_addr_t translated_address = _this->_codegen.translate(_this->_guest_context.t3);
	if (translated_address != 0) {
		_this->_guest_context.t3 = translated_address;
		restore_context_s11;
		asm("JALR zero, t3, 0");
	}
	translated_address = _this->_codegen.translate(_this->_guest_context.t3);

	// write new absolute address
	_this->_codegen.update_basic_block_address(_this->_guest_context.ra, translated_address);
	_this->_guest_context.t3 = translated_address;
	restore_context_s11;
	asm("JALR zero, t3, 0");
}

void Dispatcher::inline_translate() {
	static_assert(CodeGenerator::address_destination == encoding::RiscVRegister::t3,
				  "dispatcher::inline_translate requires t3");
	static_assert(CodeGenerator::context_address == encoding::RiscVRegister::s11,
				  "dispatcher::inline_translate requires s11");

	// capture the guest context
	capture_context_s11;

	// extract dispatcher
	register uintptr_t s11_register asm("s11");
	const auto _this = reinterpret_cast<Dispatcher*>((s11_register - offsetof(Dispatcher, _guest_context)));

	// translate address
	utils::host_addr_t translated_address = _this->_codegen.translate(_this->_guest_context.t3);
	if (translated_address != 0) {
		_this->_guest_context.t3 = translated_address;
		restore_context_s11;
		asm("JALR zero, t3, 0");
	}
	translated_address = _this->_codegen.translate(_this->_guest_context.t3);
	_this->_guest_context.t3 = translated_address;
	restore_context_s11;
	asm("JALR zero, t3, 0");
}