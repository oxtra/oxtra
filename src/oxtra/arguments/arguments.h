#ifndef OXTRA_ARGUMENTS_H
#define OXTRA_ARGUMENTS_H

#include <spdlog/common.h>
#include <cstdlib>
#include <argp.h>

namespace arguments {
	class Arguments {
	private:
		const char* _argp_program_version = "oxtra 0.1";
		const char* _argp_program_bug_address = "https://gitlab.lrz.de/lrr-tum/students/eragp-x86emu-2019";

		/**
		 * The documentation that will be printed with the usage.
		 */
		const char* _documentation = "oxtra -- oxtra x86 translator \vThanks for your interest in oxtra :).";

		/**
		 * A description of the parameter manually consumed by us.
		 */
		const char* _argument_description = "x86_EXECUTABLE";

		struct StoredArguments {
			char* guest_path;
			enum spdlog::level::level_enum spdlog_log_level;
			size_t instruction_list_size, offset_list_size, entry_list_size;
		};

		static constexpr char _instruction_list_size_id = 1;
		static constexpr char _offset_list_size_id = 2;
		static constexpr char _entry_list_size_id = 3;

		const struct argp_option _options[5] = {
				{"linstruction-size", _instruction_list_size_id, "SIZE",  0, "The size of the list containing instructions. Limit for consecutive instructions.\nThe default is 128.",   0},
				{"loffset-size",      _offset_list_size_id,      "SIZE",  0, "The size of the list containing offset. Limit for consecutive offsets.\nThe default is 128.",              0},
				{"lentry-size",       _entry_list_size_id,       "SIZE",  0, "The size of the list containing block entires. Limit for consecutive block entries.\nThe default is 128.", 0},
				{"log-level",         'l',                       "LEVEL", 0, "Specify the log level. 0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=critical, 6=off.\nThe default is 3.",  0},
				{nullptr,             0,                         nullptr, 0, nullptr,                                                                                                    0}    // This specifies the required x86 executable argument
		};

		struct argp _argp_parser;

	private:
		const char* _executable_path;

		struct StoredArguments _stored_arguments;

	public:
		Arguments(int argc, char** argv);

		Arguments(const Arguments&) = delete;

		Arguments(Arguments&&) = delete;

	public:
		const char* get_guest_path() const;

		enum spdlog::level::level_enum get_log_level() const;

		size_t get_instruction_list_size() const;

		size_t get_offset_list_size() const;

		size_t get_entry_list_size() const;

	private:
		void parse_arguments(int argc, char** argv);

		/**
		 * This method parses a single argument for argp.
		 */
		static error_t parse_opt(int key, char* arg, struct argp_state* state) {
			auto* arguments = static_cast<struct StoredArguments*>(state->input);

			switch (key) {
				case _instruction_list_size_id:
					arguments->instruction_list_size = atoi(arg);
					break;
				case _offset_list_size_id:
					arguments->offset_list_size = atoi(arg);
					break;
				case _entry_list_size_id:
					arguments->entry_list_size = atoi(arg);
					break;
				case 'l':
					arguments->spdlog_log_level = (enum spdlog::level::level_enum)(atoi(arg));
					break;

				case ARGP_KEY_NO_ARGS:
					argp_usage(state);
					break;

				case ARGP_KEY_ARG:
					if (arguments->guest_path != nullptr) {
						return ARGP_ERR_UNKNOWN;
					}

					arguments->guest_path = arg;

					break;

				default:
					return ARGP_ERR_UNKNOWN;
			}

			return 0;
		}
	};
}
#endif //OXTRA_ARGUMENTS_H
