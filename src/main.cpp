#include <fadec.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "oxtra/arguments/arguments.h"
#include "oxtra/elf/elf.h"
#include "oxtra/dispatcher/dispatcher.h"

int main(int argc, char** argv) {
	//parse the arguments
	const auto arguments = arguments::Arguments(argc, argv);
	spdlog::set_level(arguments.get_log_level());
	SPDLOG_INFO("Finished parsing the arguments.");

	//create the elf-object
	//const auto elf = elf::Elf(arguments.get_guest_path());
	const auto elf = elf::Elf(reinterpret_cast<const uint8_t*>("\x68\x78\x56\x34\x12\x50\xFF\x74\x24\x08\x9c\x66\x9c\xc3"), 14);
	SPDLOG_INFO("Finished reading and parsing elf file.");

	//create the dispatcher
	dispatcher::Dispatcher dispatcher(elf, arguments);
	SPDLOG_INFO("Finished creating and initializing various runtime-objects.");

	//startup the translation and execution of the source-code
	const auto return_value = dispatcher.run();
	spdlog::info("Returned: 0x{0:0x}", return_value);

	return return_value;
}
