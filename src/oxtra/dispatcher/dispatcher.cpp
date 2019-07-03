#include "oxtra/dispatcher/dispatcher.h"
#include <spdlog/spdlog.h>

using namespace dispatcher;
using namespace codegen;
using namespace utils;

Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args)
		: _elf(elf), _args(args), _codegen(args, elf) {

}

void Dispatcher::run() {
	const auto val = reinterpret_cast<int64_t (*)()>(_codegen.translate(_elf.get_entry_point()))();
	spdlog::warn("Returned value is: 0x{0:0x}", static_cast<uint64_t>(val));
}

void Dispatcher::host_enter() {

}

void Dispatcher::host_exit(host_addr_t addr) {

}

void Dispatcher::inline_translate() {

}