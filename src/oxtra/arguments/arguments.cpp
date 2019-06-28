#include <spdlog/spdlog.h>
#include "arguments.h"

using namespace arguments;

Arguments::Arguments(int argc, char** argv) :
		_argp_parser{_options, parse_opt, _argument_description, _documentation, nullptr, nullptr, nullptr},
		_executable_path{argv[0]},
		_stored_arguments{nullptr, spdlog::level::level_enum::warn, 128, 128, 128} {
	
	parse_arguments(argc, argv);
}

const char* Arguments::get_guest_path() const {
	return _stored_arguments.guest_path;
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
