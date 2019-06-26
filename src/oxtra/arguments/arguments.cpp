#include "arguments.h"

using namespace arguments;

Arguments::Arguments(int argc, char** argv) {

}

const char* Arguments::guest_path() const {
	return "";
}


int Arguments::debug_level() const {
	return 0;
}

bool Arguments::exit_run() const {
	return false;
}

size_t Arguments::get_instruction_list_size() const {
	return 99;
}

size_t Arguments::get_offset_list_size() const {
	return 123;
}

size_t Arguments::get_entry_list_size() const {
	return 126;
}

