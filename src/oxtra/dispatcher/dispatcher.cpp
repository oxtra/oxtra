#include "oxtra/dispatcher/dispatcher.h"

using namespace dispatcher;
using namespace codegen;

Dispatcher::Dispatcher(const elf::Elf& elf) : _elf(elf), _codegen(elf) {

}

void Dispatcher::run() {

}

void Dispatcher::host_enter() {

}

void Dispatcher::host_exit(oxtra::host_addr_t addr) {

}

void Dispatcher::inline_translate() {

}