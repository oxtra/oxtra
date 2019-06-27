#include "oxtra/dispatcher/dispatcher.h"
#include <spdlog/spdlog.h>

using namespace dispatcher;
using namespace codegen;
using namespace utils;

Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args)
		: _elf(elf), _args(args), _codegen(args, elf) {

}

void Dispatcher::run() {
	int64_t val = ((int64_t (*)())_codegen.translate(0))();
	spdlog::warn("Returned value is: {}", val >> 12);
}

void Dispatcher::host_enter() {

}

void Dispatcher::host_exit(host_addr_t addr) {

}

void Dispatcher::inline_translate() {

}