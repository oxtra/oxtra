#include <spdlog/spdlog.h>
#include "arguments.h"

using namespace arguments;

const char* argp_program_version = "oxtra 0.5 [alpha]";
const char* argp_program_bug_address = "https://gitlab.lrz.de/lrr-tum/students/eragp-x86emu-2019";

Arguments::Arguments(int argc, char** argv) :
		_executable_path{argv[0]},
		_stored_arguments{nullptr, std::vector<std::string>(), spdlog::level::level_enum::warn, false,
						  0x200000, 0x1000, 0x200, 0x40} {

	parse_arguments(argc, argv);
}

const char* Arguments::get_guest_path() const {
	return _stored_arguments.guest_path;
}

std::vector<std::string> Arguments::get_guest_arguments() const {
	return _stored_arguments.guest_arguments;
}

enum spdlog::level::level_enum Arguments::get_log_level() const {
	return _stored_arguments.spdlog_log_level;
}

size_t Arguments::get_stack_size() const {
	return _stored_arguments.stack_size;
}

size_t Arguments::get_instruction_list_size() const {
	return _stored_arguments.instruction_list_size;
}

size_t Arguments::get_offset_list_size() const {
	return _stored_arguments.offset_list_size;
}

size_t Arguments::get_entry_list_size() const {
	return _stored_arguments.entry_list_size;
}

bool Arguments::get_debugging() const {
	return _stored_arguments.debugging;
}

void Arguments::parse_arguments(int argc, char** argv) {
	// initialize the argument-parser
	argp parser = {_options, parse_opt, _argument_description, _documentation, nullptr, nullptr, nullptr};

	// parse the arguments
	argp_parse(&parser, argc, argv, 0, nullptr, &_stored_arguments);
}

/**
 * This method parses a given string and ensures that it is >= the min_value.
 * If there was an error while parsing, the failure_string will be printed.
 * @return The parsed value. If there was an error, the application will be exited.
 */
static long parse_string(struct argp_state* state, char* str, int min_value, const char* failure_string) {
	char* end_parse;
	long parsed_value = strtol(str, &end_parse, 10);
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
		argp_failure(state, 1, 0, "%s: %s. %s %d-%d.", failure_string, str, "Please specify a value between", min_value,
					 max_value);
	}

	return ret;
}

static std::vector<std::string> string_split(const std::string& string, const char delimiter) {
	std::vector<std::string> tokens;

	size_t current = 0, previous = 0;
	do {
		current = string.find(delimiter, previous);
		const auto token = string.substr(previous, current - previous);

		// check if the token currently is a string and has not been ended yet
		// it has been ended, if there is a " but not a \" at the end
		if (!tokens.empty() && tokens.back().find_first_of('"') == 0
			&& (tokens.back().find_last_of('"') != tokens.back().length() - 1 ||
				tokens.back().find_last_of('\\') == tokens.back().length() - 2)) {
			tokens.back() += delimiter + token;
		} else if (token.length() > 0) {
			tokens.push_back(token);
		}

		previous = current + 1;
	} while (current != std::string::npos);

	return tokens;
}

error_t Arguments::parse_opt(int key, char* arg, struct argp_state* state) {
	auto* arguments = static_cast<struct StoredArguments*>(state->input);

	switch (key) {
		case 'a':
			arguments->guest_arguments = string_split(arg, ' ');
			break;
		case 'l': {
			int parsed = parse_string(state, arg, 0, 6, "Illegal log level");
			arguments->spdlog_log_level = static_cast<enum spdlog::level::level_enum>(parsed);
			break;
		}
		case 'd':
			if (strcasecmp("false", arg) == 0 || strcasecmp("0", arg) == 0)
				arguments->debugging = false;
			else if (strcasecmp("true", arg) == 0 || strcasecmp("1", arg) == 0)
				arguments->debugging = true;
			else
				argp_failure(state, 1, 0, "%s: %s. %s.", "Illegal debugging-behavior", arg, "Allowed are: true, 1, false, 0");
			break;
		case 's':
			arguments->stack_size = parse_string(state, arg, 1, "Illegal size, must be a positive integer.");
			break;
		case 'i':
			arguments->instruction_list_size = parse_string(state, arg, 1, "Illegal size, must be a positive integer.");
			break;
		case 'o':
			arguments->offset_list_size = parse_string(state, arg, 1, "Illegal size, must be a positive integer.");
			break;
		case 'e':
			arguments->entry_list_size = parse_string(state, arg, 1, "Illegal size, must be a positive integer.");
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
