#ifndef OXTRA_DISPATCHER_H
#define OXTRA_DISPATCHER_H

#include "oxtra/utils/types.h"
#include "oxtra/arguments/arguments.h"
#include "oxtra/codegen/codegen.h"
#include "oxtra/codegen/helper.h"
#include "oxtra/elf/elf.h"
#include "execution_context.h"

namespace dispatcher {
	static_assert(codegen::helper::address_destination == encoding::RiscVRegister::t3,
				  "dispatcher::reroute_static, reroute_dynamic requires t3");
	static_assert(codegen::helper::context_address == encoding::RiscVRegister::s11,
				  "dispatcher::reroute_static, reroute_dynamic requires s11");

	class Dispatcher {
	private:
		ExecutionContext _context;
		const elf::Elf& _elf;
		const arguments::Arguments& _args;
		char** _envp;
		codegen::CodeGenerator _codegen;

	private:
		static constexpr size_t tlb_size = 0x80;

	public:
		Dispatcher(const elf::Elf& elf, const arguments::Arguments& args, char** envp);

		Dispatcher(const Dispatcher&) = delete;
		Dispatcher(Dispatcher&&) = delete;

	public:
		/**
		 * Initializes execution of the program.
		 * @return exit_code of the program
		 */
		long run();

		/**
		 * Exits the guest-program with the given exit-code
		 * @param exit_code the exit-code to be returned
		 */
		static void guest_exit(long exit_code);

		/**
		 * Exits the guest-program with a given fault-string and exit-code
		 * @param fault_string the fault-string to be thrown
		 * @param exit_code the exit-code to be returned
		 */
		static void fault_exit(const char* fault_string, long exit_code = -1);

		static ExecutionContext* execution_context();

	private:
		/**
		 * Initialize the guest context with an (x64) ABI conform stack and registers.
		 */
		std::tuple<uintptr_t, uintptr_t, uintptr_t> init_guest_context();

		/**
		 * Called instead of a syscall instruction.
		 * Either handles the syscall or forwards it to the kernel.
		 */
		static void syscall_handler();

		/**
		 * Emulates the syscall if possible, or fault exits otherwise.
		 */
		static void emulate_syscall(ExecutionContext* context);

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
		 * Translates a return address, saves it in the call table and jumps to it.
		 */
		static void reroute_return();

		/**
		 * Entry into the guest context. (Reversed by guest_exit or fault_exit).
		 * @param context A pointer to {guest context, host context}.
		 * @param entry The riscv entry point.
		 * @param fault_string A pointer, which might store the address to the fault_string, or zero
		 * @return The exit_code parameter.
		 */
		static long guest_enter(ExecutionContext* context, utils::guest_addr_t entry, const char** fault_string);
	};
}

#endif //OXTRA_DISPATCHER_H
