#ifndef OXTRA_DISPATCHER_H
#define OXTRA_DISPATCHER_H

#include "oxtra/types.h"
#include "oxtra/codegen/codegen.h"
#include "oxtra/elf/elf.h"

namespace dispatcher {
	struct Context {
		//TODO: implement context
	};

	class Dispatcher {
		friend codegen::CodeGenerator;

	private:
		static Dispatcher *_this;

	private:
		const elf::Elf &_elf;
		codegen::CodeGenerator _codegen;
		Context _guest_context;
		Context _host_context;

	public:
		Dispatcher(const elf::Elf &elf);

		Dispatcher(Dispatcher &) = delete;

		Dispatcher(Dispatcher &&) = delete;

	public:
		void run();

	private:
		static void host_enter();

		static void host_exit(oxtra::real_t addr);

		static void inline_translate();
	};
}

#endif //OXTRA_DISPATCHER_H
