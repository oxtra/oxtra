#include <fadec.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "oxtra/arguments/arguments.h"
#include "oxtra/elf/elf.h"
#include "oxtra/dispatcher/dispatcher.h"

int main(int argc, char** argv) {
	// parse the arguments
	const auto arguments = arguments::Arguments(argc, argv);
	spdlog::set_level(arguments.get_log_level());
	spdlog::debug("finished parsing the arguments.");

	// create the elf-object
	const auto elf = elf::Elf(arguments.get_guest_path());
	spdlog::debug("finished reading and parsing elf file.");

	// create the dispatcher
	dispatcher::Dispatcher dispatcher(elf, arguments);
	spdlog::debug("finished creating and initializing various runtime-objects.");

	// startup the translation and execution of the source-code
	const auto return_value = dispatcher.run();
	spdlog::info("guest returned: {0:x} ({0:d})", return_value);

	return return_value;
}
