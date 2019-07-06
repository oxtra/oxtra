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
	const auto elf = elf::Elf(arguments.get_guest_path());
	SPDLOG_INFO("Finished reading and parsing elf file.");

	spdlog::warn("elf: {0:0x}", (intptr_t)elf.get_address_delta());
	spdlog::warn("elf: {0:0x}", elf.get_base_vaddr());
	spdlog::warn("elf: {}", elf.resolve_vaddr(elf.get_base_vaddr()));

	//create the dispatcher
	dispatcher::Dispatcher dispatcher(elf, arguments);
	SPDLOG_INFO("Finished creating and initializing various runtime-objects.");

	dispatcher::Context context;
	register uintptr_t s11_register asm("s11") = reinterpret_cast<uintptr_t>(&context);
	capture_context_s11;

	spdlog::info("old context: {}", context);

	//startup the translation and execution of the source-code
	dispatcher.run();

	int* test = (int*)0;
	test[0] = 0;

	printf("Hello World-a");
	fflush(stdout);

	return 0;
}
