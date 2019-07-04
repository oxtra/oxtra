#include <fadec.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#include "oxtra/arguments/arguments.h"
#include "oxtra/elf/elf.h"
#include "oxtra/dispatcher/dispatcher.h"


/* movabs r10, 0xf00dbabe
	 * movsx rax, bl
	 * movzx rbx, BYTE PTR [rax+r15*8+0x11223344]
	 * movsx  rbx,BYTE PTR [rax+rbx*8+0x12345678]
	 * mov    BYTE PTR [rax+rbx*8+0x12345678],dh
	 * mov rax, rbx
	 * mov eax, ebx
	 * mov ax, bx
	 * mov al, bl
	 * mov ah, bh
	 */
uint8_t buffer[] = {
		0x49, 0xBA, 0xBE, 0xBA, 0x0D, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x48, 0x0f, 0xbe, 0xc3, 0x4a, 0x0f,
		0xb6, 0x9c, 0xf8, 0x44, 0x33, 0x22, 0x11, 0x48, 0x0f, 0xbe, 0x9c, 0xd8, 0x78, 0x56, 0x34, 0x12,
		0x88, 0xb4, 0xd8, 0x78, 0x56, 0x34, 0x12, 0x48, 0x89, 0xd8, 0x89, 0xd8, 0x66, 0x89, 0xd8, 0x88,
		0xd8, 0x88, 0xfc, 0xc3
};


int main(int argc, char** argv) {
	//parse the arguments
	const auto arguments = arguments::Arguments(argc, argv);
	spdlog::set_level(arguments.get_log_level());
	SPDLOG_INFO("Finished parsing the arguments.");

	//create the elf-object
	const auto elf = elf::Elf(buffer, sizeof(buffer));
	//const auto elf = elf::Elf(arguments.get_guest_path());
	SPDLOG_INFO("Finished reading and parsing elf file.");

	//create the dispatcher
	dispatcher::Dispatcher dispatcher(elf, arguments);
	SPDLOG_INFO("Finished creating and initializing various runtime-objects.");

	//startup the translation and execution of the source-code
	dispatcher.run();
	return 0;
}
