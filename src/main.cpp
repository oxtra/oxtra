#include <fadec.h>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "oxtra/arguments/arguments.h"
#include "oxtra/elf/elf.h"
#include "oxtra/dispatcher/dispatcher.h"

int main(int argc, char** argv) {
	//parse the arguments
	const auto arguments = arguments::Arguments(argc, argv);
	if(arguments.exit_run())
		return 0;
	SPDLOG_INFO("Finished parsing the arguments.");

	//create the elf-object
	const auto elf = elf::Elf(arguments.guest_path());
	SPDLOG_INFO("Finished reading and parsing elf file.");

	//create the dispatcher
	dispatcher::Dispatcher dispatcher(elf, arguments);
	SPDLOG_INFO("Finished creating and initializing various runtime-objects.");

	//startup the translation and execution of the source-code
	dispatcher.run();
	return 0;
}
