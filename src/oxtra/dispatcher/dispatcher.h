#ifndef OXTRA_DISPATCHER_H
#define OXTRA_DISPATCHER_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codegen.h"
#include "oxtra/elf/elf.h"
#include "context.h"

namespace dispatcher {
	class Dispatcher {
	private:
		const elf::Elf& _elf;
		const arguments::Arguments& _args;
		codegen::CodeGenerator _codegen;
		Context _guest_context, _host_context;

		//static_assert(offset_of<Dispatcher, Dispatcher::Context, &Dispatcher::_context>() == 0xA0,
		//        "Changing this value requires changing the assembly of Context::capture");

	public:
		Dispatcher(const elf::Elf& elf, const arguments::Arguments& args);

		Dispatcher(const Dispatcher&) = delete;

		Dispatcher(Dispatcher&&) = delete;

	public:
		void run();

	private:
		static void host_call();

		static void inline_translate();
	};
}

#endif //OXTRA_DISPATCHER_H
