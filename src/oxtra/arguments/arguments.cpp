#include "arguments.h"

using namespace arguments;

Arguments::Arguments(int argc, char** argv) {

}

char* Arguments::guest_path() const {
	return nullptr;
}


int Arguments::debug_level() const {
	return 0;
}

bool Arguments::is_version() const {
	return false;
}

bool Arguments::is_help() const {
	return false;
}

size_t Arguments::get_instruction_list_size() const {
	return 0;
}

size_t Arguments::get_offset_list_size() const {
	return 0;
}

size_t Arguments::get_entry_list_size() const {
	return 0;
}

