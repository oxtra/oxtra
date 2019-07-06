#include <fadec.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "oxtra/arguments/arguments.h"
#include "oxtra/elf/elf.h"
#include "oxtra/dispatcher/dispatcher.h"

#include <iostream>
using namespace std;

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
	register uintptr_t stack_ptr asm("sp");
	register uintptr_t frame_ptr asm("s0");
	register uintptr_t s11_register asm("s11") = reinterpret_cast<uintptr_t>(&context);
	capture_context_s11;
	printf("stack_ptr: %llx\n", stack_ptr);
	printf("frame_ptr: %llx\n", frame_ptr);
	printf("dispatcher: %llx\n", &dispatcher);
	fflush(stdout);

	//startup the translation and execution of the source-code
	printf("returned: %llx\n", dispatcher.run());


	printf("stack_ptr: %llx\n", stack_ptr);
	printf("frame_ptr: %llx\n", frame_ptr);
	printf("Hello World-a\n");
	fflush(stdout);

	//stack_ptr = 0x40007ffc20;
	//frame_ptr = 0x40008001e0;
	//asm("li fp, 0x40008001e0");

	return 0;
}
