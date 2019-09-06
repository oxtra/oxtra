#ifndef OXTRA_ARGUMENTS_H
#define OXTRA_ARGUMENTS_H

#include <spdlog/common.h>
#include <cstdlib>
#include <argp.h>

namespace arguments {
	enum class StepMode {
		none,
		x86,
		riscv
	};

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
			enum spdlog::level::level_enum spdlog_log_level;
			enum StepMode step_mode;
			size_t stack_size;
			size_t instruction_list_size, offset_list_size, entry_list_size;
		};

		static constexpr char _instruction_list_size_id = 1;
		static constexpr char _offset_list_size_id = 2;
		static constexpr char _entry_list_size_id = 3;

		/**
		 * This struct contains all options and their identifiers (+ definition strings).
		 * Unprintable ASCII strings will not be able to be accessed (only long variant available).
		 */
		const struct argp_option _options[8] = {
				{"args",         'a',                       "\"ARGUMENTS...\"", 0, "Specify the arguments that will be passed to the x86 executable. The default is no arguments",                                                          0},
				{"log-level",    'l',                       "LEVEL",            0, "Specify the log level. 0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=critical, 6=off.\nThe default is 3.",                                               0},
				{"debug",        'd',                       "STEP_MODE",        0, "Specify the stepping behavior. Allowed step modes are: none, x86, or riscv specifying the granularity of possible breakpoints.\nThe default ist none.", 0},
				{"stack-size",   's',                       "SIZE",             0, "The size of the stack in decimal.\nThe default size is 52,428,8000 (0x3200000) bytes.",                                                                                                     0},
				{"linst-size",   _instruction_list_size_id, "SIZE",             0, "The size of the list containing instructions. Limit for consecutive instructions.\nThe default is 128.",                                                0},
				{"loffset-size", _offset_list_size_id,      "SIZE",             0, "The size of the list containing offset. Limit for consecutive offsets.\nThe default is 128.",                                                           0},
				{"lentry-size",  _entry_list_size_id,       "SIZE",             0, "The size of the list containing block entires. Limit for consecutive block entries.\nThe default is 128.",                                              0},
				// This specifies the required x86 executable argument
				{nullptr,        0,                         nullptr,            0, nullptr,                                                                                                                                                 0}
		};

		const struct argp _argp_parser;

	private:
		const char* _executable_path;

		struct StoredArguments _stored_arguments;

	public:
		Arguments(int argc, char** argv);

		Arguments(Arguments&) = delete;

		Arguments(const Arguments&&) = delete;

	public:
		const char* get_guest_path() const;

		std::vector<std::string> get_guest_arguments() const;

		enum spdlog::level::level_enum get_log_level() const;

		size_t get_stack_size() const;

		size_t get_instruction_list_size() const;

		size_t get_offset_list_size() const;

		size_t get_entry_list_size() const;

		StepMode get_step_mode() const;

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
