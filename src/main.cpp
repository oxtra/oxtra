#include <fadec.h>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "oxtra/arguments/arguments.h"
#include "oxtra/elf/elf.h"
#include "oxtra/dispatcher/dispatcher.h"

//TODO: implement argument-parser-class & way to pass arguments to dispatcher

int main(int argc, char** argv) {
	//create the elf-object
	if (argc < 2) {
		spdlog::error("invalid arguments");
		return 0;
	}

	const auto arguments = arguments::Arguments(argc, argv);

	const auto elf = elf::Elf(arguments.guest_path());
	SPDLOG_INFO("Finished reading and parsing elf file.");

	//create the dispatcher
	dispatcher::Dispatcher dispatcher(elf);
	SPDLOG_INFO("Finished creating and initializing various runtime-objects.");

	//startup the translation and execution of the source-code
	dispatcher.run();
	return 0;
}
