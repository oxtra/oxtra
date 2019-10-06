#ifndef OXTRA_ARGUMENTS_H
#define OXTRA_ARGUMENTS_H

#include <cstdlib>
#include <argp.h>
#include <vector>
#include <string>

#include "oxtra/logger/logger.h"

namespace arguments {
	class Arguments {
	private:
		/**
		 * The documentation that will be printed with the usage.
		 */
		static constexpr const char* _documentation = "oxtra -- oxtra x86 translator\vThanks for your interest in oxtra :).";

		/**
		 * A description of the parameter manually consumed by us.
		 */
		static constexpr const char* _argument_description = "x86_EXECUTABLE";

		/**
		 * This struct contains the actual arguments that have been extracted.
		 */
		struct StoredArguments {
			char* guest_path;
			std::vector<std::string> guest_arguments;
			uint8_t log_level;
			uint8_t debugging;
			size_t stack_size;
			size_t instruction_list_size, offset_list_size, entry_list_size;
		};

		/**
		 * This struct contains all options and their identifiers (+ definition strings).
		 * Unprintable ASCII strings will not be able to be accessed (only long variant available).
		 */
		static const unsigned char dbk_key = 0x80;
		const struct argp_option _options[8] = {
				{"args",         'a',     "\"ARGUMENTS...\"", 0, "Specify the arguments that will be passed to the x86 executable. The default is no arguments",                             0},
				{"debug",        dbk_key, "MODE",            0, "Specify to attach and enable the debugger. 0=disabled, 1=lightweight, 2=riscv-enabled. The default is 0.", 0},
				{"lentry-size",  'e',     "SIZE",             0, "The size of the list containing block entires. Limit for consecutive block entries. The default is 64.",                   0},
				{"linst-size",   'i',     "SIZE",             0, "The size of the list containing instructions. Limit for generated RISCV instructions in a block. The default is 4096.",    0},
				{"log-level",    'l',     "LEVEL",            0, "Specify the attributes to log. 0=riscv, 1=x86, 2=translated, 3=reroutes, 4=syscalls, 5=return value, 6=all. The default is none.",                   0},
				{"loffset-size", 'o',     "SIZE",             0, "The size of the list containing offset. Limit for consecutive offsets. The default is 512.",                               0},
				{"stack-size",   's',     "SIZE",             0, "The size of the stack in decimal. The default size is 2MiB (0x200000).",                                                   0},
				// This specifies the required x86 executable argument
				{nullptr,        0,       nullptr,            0, nullptr,                                                                                                                    0}
		};

	private:
		const char* _executable_path;

		struct StoredArguments _stored_arguments;

	public:
		Arguments(int argc, char** argv);

		Arguments(Arguments&) = delete;

		Arguments(const Arguments&&) = delete;

	public:
		const char* get_guest_path() const;

		const std::vector<std::string>& get_guest_arguments() const;

		uint8_t get_log_level() const;

		size_t get_stack_size() const;

		size_t get_instruction_list_size() const;

		size_t get_offset_list_size() const;

		size_t get_entry_list_size() const;

		uint8_t get_debugging() const;

	private:
		/**
		 * This method will parse the specified arguments, sets the according fields in _stored_arguments and
		 * may even terminate the application (if arguments are missing, help argument etc.)
		 * @param argc The argument count
		 * @param argv The array of arguments including the name of the executable at the 0th position.
		 */
		void parse_arguments(int argc, char** argv);

		/**
		 * This method parses a single argument for argp.
		 * See here: https://www.gnu.org/software/libc/manual/html_node/Argp.html for more information.
		 */
		static error_t parse_opt(int key, char* arg, struct argp_state* state);
	};
}
#endif //OXTRA_ARGUMENTS_H
