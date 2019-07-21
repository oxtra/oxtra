#include <spdlog/spdlog.h>
#include "arguments.h"

using namespace arguments;

const char* argp_program_version = "oxtra 0.4 [alpha]";
const char* argp_program_bug_address = "https://gitlab.lrz.de/lrr-tum/students/eragp-x86emu-2019";

Arguments::Arguments(int argc, char** argv) :
		_argp_parser{_options, parse_opt, _argument_description, _documentation, nullptr, nullptr, nullptr},
		_executable_path{argv[0]},
		_stored_arguments{nullptr, std::vector<std::string>(), spdlog::level::level_enum::warn, 128, 128, 128} {

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

size_t Arguments::get_instruction_list_size() const {
	return _stored_arguments.instruction_list_size;
}

size_t Arguments::get_offset_list_size() const {
	return _stored_arguments.offset_list_size;
}

size_t Arguments::get_entry_list_size() const {
	return _stored_arguments.entry_list_size;
}

void Arguments::parse_arguments(int argc, char** argv) {
	argp_parse(&_argp_parser, argc, argv, 0, nullptr, &_stored_arguments);
}

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

static std::vector<std::string> string_split(const std::string& string, const char delimiter) {
	std::vector<std::string> tokens;

	size_t current = 0, previous = 0;
	do {
		current = string.find(delimiter, previous);
		const auto token = string.substr(previous, current - previous);

		// check if the token currently is a string and has not been ended yet
		// it has been ended, if there is a " but not a \" at the end
		if (!tokens.empty() && tokens.back().find_first_of('"') == 0
		&& (tokens.back().find_last_of('"') != tokens.back().length() - 1 || tokens.back().find_last_of('\\') == tokens.back().length() - 2)) {
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
		case _instruction_list_size_id:
			arguments->instruction_list_size = parse_string(state, arg, 1, "Illegal size");
			break;
		case _offset_list_size_id:
			arguments->offset_list_size = parse_string(state, arg, 1, "Illegal size");
			break;
		case _entry_list_size_id:
			arguments->entry_list_size = parse_string(state, arg, 1, "Illegal size");
			break;
		case 'l': {
			int parsed = parse_string(state, arg, 0, 6, "Illegal log level");
			arguments->spdlog_log_level = static_cast<enum spdlog::level::level_enum>(parsed);
			break;
		}
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
