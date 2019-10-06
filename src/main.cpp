#include <fadec.h>

#include <spdlog/spdlog.h>

#include "oxtra/arguments/arguments.h"
#include "oxtra/elf/elf.h"
#include "oxtra/dispatcher/dispatcher.h"

int main(int argc, char** argv, char** envp) {
	// parse the arguments and set the spdlog-pattern
	const auto arguments = arguments::Arguments(argc, argv);
	spdlog::set_pattern("[%H:%M:%S.%e %^%l%$] %v");
	spdlog::set_level(arguments.get_log_level());

	// create the elf-object
	const auto elf = elf::Elf(arguments.get_guest_path());

	// create the dispatcher
	dispatcher::Dispatcher dispatcher(elf, arguments, envp);

	// startup the translation and execution of the source-code
	const auto return_value = dispatcher.run();
	spdlog::info("guest returned: {0:#x} ({1:d})", static_cast<unsigned long>(return_value), return_value);

	return return_value;
}
