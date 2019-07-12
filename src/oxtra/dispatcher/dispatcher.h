#ifndef OXTRA_DISPATCHER_H
#define OXTRA_DISPATCHER_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codegen.h"
#include "oxtra/elf/elf.h"
#include "context.h"

namespace dispatcher {
	static_assert(codegen::CodeGenerator::address_destination == encoding::RiscVRegister::t3,
				  "dispatcher::reroute_static, reroute_dynamic, run requires t3");
	static_assert(codegen::CodeGenerator::context_address == encoding::RiscVRegister::s11,
				  "dispatcher::reroute_static, reroute_dynamic, run requires s11");
	static_assert(codegen::CodeGenerator::reroute_dynamic_address == encoding::RiscVRegister::s9,
				  "dispatcher::run requires s9");
	static_assert(codegen::CodeGenerator::reroute_static_address == encoding::RiscVRegister::s8,
				  "dispatcher::run requires s8");

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

		/**
		 * Emulates a exit syscall by switching back to the host context. (Reverses changes of guest_enter).
		 * @return The exit_code parameter.
		 */
		static int guest_exit();
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

		/**
		 * Entry into the guest context. (Reversed by guest_exit).
		 * @param context A pointer to {guest context, host context}.
		 * @param entry The riscv entry point.
		 * @return The exit_code parameter.
		 */
		static int guest_enter(Context* context, utils::guest_addr_t entry);
	};
}

#endif //OXTRA_DISPATCHER_H
