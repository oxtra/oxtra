#ifndef OXTRA_ARGUMENTS_H
#define OXTRA_ARGUMENTS_H

#include <cstdlib>

namespace arguments {
	class Arguments {
	public:
		Arguments(int argc, char** argv);

		Arguments(const Arguments&) = delete;

		Arguments(Arguments&&) = delete;

	public:
		char* guest_path() const;

		int debug_level() const;

		bool is_version() const;

		bool is_help() const;

		size_t get_instruction_list_size() const;

		size_t get_offset_list_size() const;

		size_t get_entry_list_size() const;
	};
}
#endif //OXTRA_ARGUMENTS_H
