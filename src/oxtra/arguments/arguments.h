#ifndef OXTRA_ARGUMENTS_H
#define OXTRA_ARGUMENTS_H

#include <spdlog/common.h>
#include <cstdlib>
#include <argp.h>

namespace arguments {
	class Arguments {
	private:
		static constexpr const char* _argp_program_version = "oxtra 0.1";
		static constexpr const char* _argp_program_bug_address = "https://gitlab.lrz.de/lrr-tum/students/eragp-x86emu-2019";

		/**
		 * The documentation that will be printed with the usage.
		 */
		static constexpr const char* _documentation = "oxtra -- oxtra x86 translator \vThanks for your interest in oxtra :).";

		/**
		 * A description of the parameter manually consumed by us.
		 */
		static constexpr const char* _argument_description = "x86_EXECUTABLE";

		/**
		 * This struct contains the actual arguments that have been exctracted.
		 */
		struct StoredArguments {
			char* guest_path;
			enum spdlog::level::level_enum spdlog_log_level;
			size_t instruction_list_size, offset_list_size, entry_list_size;
		};

		static constexpr char _instruction_list_size_id = 1;
		static constexpr char _offset_list_size_id = 2;
		static constexpr char _entry_list_size_id = 3;

		/**
		 * This struct contains all options and their identifiers (+ definition strings).
		 * Unprintable ASCII strings will not be able to be accessed (only long variant available).
		 */
		const struct argp_option _options[5] = {
				{"linstruction-size", _instruction_list_size_id, "SIZE",  0, "The size of the list containing instructions. Limit for consecutive instructions.\nThe default is 128.",   0},
				{"loffset-size",      _offset_list_size_id,      "SIZE",  0, "The size of the list containing offset. Limit for consecutive offsets.\nThe default is 128.",              0},
				{"lentry-size",       _entry_list_size_id,       "SIZE",  0, "The size of the list containing block entires. Limit for consecutive block entries.\nThe default is 128.", 0},
				{"log-level",         'l',                       "LEVEL", 0, "Specify the log level. 0=trace, 1=debug, 2=info, 3=warn, 4=error, 5=critical, 6=off.\nThe default is 3.",  0},
				// This specifies the required x86 executable argument
				{nullptr,             0,                         nullptr, 0, nullptr,                                                                                                    0}
		};

		const struct argp _argp_parser;

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
		/**
		 * This method will parse the specified arguments, sets the according fields in _stored_arguments and may even exit the program (if arguments are missing, help argument etc.)
		 * @param argc The argument count
		 * @param argv The array of arguments including the name of the executable at the 0th position.
		 */
		void parse_arguments(int argc, char** argv);

		/**
		 * This method parses a given string and ensures that it is >= the min_value.
		 * If there was an error while parsing, the failure_string will be printed.
		 * @return The parsed value. If there was an error, the application will be exited.
		 */
		static int parse_string(struct argp_state* state, char* str, int min_value, const char* failure_string) {
			char* end_parse;
			int parsed_value = strtol(str, &end_parse, 10);
			if (*end_parse != '\0' || parsed_value < min_value) {
				argp_failure(state, 1, 0, "%s: %s", failure_string, str);
			}

			return parsed_value;
		}

		/**
 		* This method parses a given string and ensures that it is >= the min_value and <= the max_value.
 		* If there was an error while parsing, the failure_string will be printed.
 		* @return The parsed value. If there was an error, the application will be exited.
 		*/
		static int parse_string(struct argp_state* state, char* str,
								int min_value, int max_value, const char* failure_string) {
			int ret = parse_string(state, str, min_value, failure_string);

			if (ret > max_value) {
				argp_failure(state, 1, 0, "%s: %s", failure_string, str);
			}

			return ret;
		}

		/**
		 * This method parses a single argument for argp.
		 * See here: https://www.gnu.org/software/libc/manual/html_node/Argp.html for more information.
		 */
		static error_t parse_opt(int key, char* arg, struct argp_state* state) {
			auto* arguments = static_cast<struct StoredArguments*>(state->input);
			int parsed;

			switch (key) {
				case _instruction_list_size_id:
					arguments->instruction_list_size = parse_string(state, arg, 1, "Illegal size");
					break;
				case _offset_list_size_id:
					arguments->offset_list_size = parse_string(state, arg, 1, "Illegal size");
					break;
				case _entry_list_size_id:
					arguments->entry_list_size = parse_string(state, arg, 1, "Illegal size");
					break;
				case 'l':
					parsed = parse_string(state, arg, 0, 6, "Illegal log level");
					arguments->spdlog_log_level = static_cast<enum spdlog::level::level_enum>(parsed);
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
