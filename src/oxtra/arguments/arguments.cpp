#include <spdlog/spdlog.h>
#include "arguments.h"

using namespace arguments;

const char* argp_program_version = "oxtra 0.1";
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

	size_t current, previous = 0;
	current = string.find(delimiter);
	while (current != std::string::npos) {
		const auto token = string.substr(previous, current - previous);
		if (token.length() > 0) {
			tokens.push_back(token);
		}

		previous = current + 1;
		current = string.find(delimiter, previous);
	}

	tokens.push_back(string.substr(previous, current - previous));

	return tokens;
}

error_t Arguments::parse_opt(int key, char* arg, struct argp_state* state) {
	auto* arguments = static_cast<struct StoredArguments*>(state->input);

	switch (key) {
		case 'a':
			arguments->guest_arguments = string_split(arg, ' ');
			for (auto const& value: arguments->guest_arguments) {
				printf("%s\n", value.c_str());
			}
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
