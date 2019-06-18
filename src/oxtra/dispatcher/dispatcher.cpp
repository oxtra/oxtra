#include "oxtra/dispatcher/dispatcher.h"

using namespace dispatcher;
using namespace codegen;
using namespace utils;

Dispatcher::Dispatcher(const elf::Elf& elf, const arguments::Arguments& args)
		: _elf(elf), _args(args), _codegen(elf, args) {

}

void Dispatcher::run() {

}

void Dispatcher::host_enter() {

}

void Dispatcher::host_exit(host_addr_t addr) {

}

void Dispatcher::inline_translate() {

}