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
		friend codegen::CodeGenerator;

	private:
		static Dispatcher* _this;

	private:
		const elf::Elf& _elf;
		const arguments::Arguments& _args;
		codegen::CodeGenerator _codegen;
		Context _context;

		//static_assert(offset_of<Dispatcher, Dispatcher::Context, &Dispatcher::_context>() == 0xA0,
		//        "Changing this value requires changing the assembly of Context::store");

	public:
		Dispatcher(const elf::Elf& elf, const arguments::Arguments& args);

		Dispatcher(const Dispatcher&) = delete;

		Dispatcher(Dispatcher&&) = delete;

	public:
		void run();

		static void host_enter();
	private:

		static void host_exit(utils::host_addr_t addr);

		static void inline_translate();
	};
}

#endif //OXTRA_DISPATCHER_H
