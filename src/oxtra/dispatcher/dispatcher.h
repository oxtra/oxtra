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
		Context _guest_context, _host_context;
		const elf::Elf& _elf;
		const arguments::Arguments& _args;
		codegen::CodeGenerator _codegen;

	public:
		Dispatcher(const elf::Elf& elf, const arguments::Arguments& args);

		Dispatcher(const Dispatcher&) = delete;

		Dispatcher(Dispatcher&&) = delete;

	public:
		int run();

	private:
		/**
		 * Translates a guest branch address and reroutes the control flow to the branch target
		 * by rewriting the branch instruction.
		 */
		static void reroute_static();

		/**
		 * Translates a guest branch address and reroutes the control flow to the branch target
		 * by jumping to the target in software.
		 */
		static void reroute_dynamic();
	};
}

#endif //OXTRA_DISPATCHER_H
