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
		 * exits the guest-program with the given exit-code
		 * @param exit_code the exit-code to be returned
		 */
		static void guest_exit(int exit_code);

		/**
		 * exits the guest-program with a given fault-string and exit-code
		 * @param fault_string the fault-string to be thrown
		 * @param exit_code the exit-code to be returned
		 */
		static void fault_exit(const char* fault_string, int exit_code = -1);
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
		 * Entry into the guest context. (Reversed by guest_exit or fault_exit).
		 * @param context A pointer to {guest context, host context}.
		 * @param entry The riscv entry point.
		 * @param fault_string A pointer, which might store the address to the fault_string, or zero
		 * @return The exit_code parameter.
		 */
		static int guest_enter(Context* context, utils::guest_addr_t entry, const char** fault_string);
	};
}

#endif //OXTRA_DISPATCHER_H
