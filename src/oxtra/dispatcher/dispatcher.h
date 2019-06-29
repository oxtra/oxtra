#ifndef OXTRA_DISPATCHER_H
#define OXTRA_DISPATCHER_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codegen.h"
#include "oxtra/elf/elf.h"

namespace dispatcher {
	class Dispatcher {
	private:
		struct Context {
			//TODO: implement context
		};

	private:
		friend codegen::CodeGenerator;

	private:
		static Dispatcher* _this;

	private:
		const elf::Elf& _elf;
		const arguments::Arguments& _args;
		codegen::CodeGenerator _codegen;
		Context _guest_context;
		Context _host_context;

	public:
		Dispatcher(const elf::Elf& elf, const arguments::Arguments& args);

		Dispatcher(Dispatcher&) = delete;

		Dispatcher(Dispatcher&&) = delete;

	public:
		void run();

	private:
		static void host_enter();

		static void host_exit(utils::host_addr_t addr);

		static void inline_translate();
	};
}

#endif //OXTRA_DISPATCHER_H