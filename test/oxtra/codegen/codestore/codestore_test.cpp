/* define FAKE_GUEST to use a default throw and not Dispatcher::fault_exit */

#include "../../../catch2/catch.hpp"
#include "oxtra/codegen/codestore/codestore.h"
#include <string>
#include <unistd.h>
#include <string.h>
#include "oxtra/elf/elf.h"
#include "oxtra/arguments/arguments.h"
#include "fadec.h"
#include "oxtra/utils/types.h"

using namespace codegen;
using namespace codestore;

/* These arrays describe the x86 instructions, which the CodeStore will have to track.
 * The first and the fourth instruction must not be 1 byte in size. Otherwise the
 * miss-alignment test will be useless. */
/* mov rbp, rsp
 * push rbp
 * and esp, 0xfffffff0
 * sub rsp, 0x168
 * xor rax, rsp
 * mov [rsp+0x164], rax
 * push rsi
 * push rdi
 * sub rsp, 0x0c
 * */
constexpr uint8_t instruction_buffer[] = {0x48, 0x89, 0xE5, 0x55, 0x83, 0xE4, 0xF0, 0x48, 0x81, 0xEC, 0x68, 0x01, 0x00,
										  0x00, 0x48, 0x31, 0xE0, 0x48, 0x89, 0x84, 0x24, 0x64, 0x01, 0x00, 0x00, 0x56,
										  0x57, 0x48, 0x83, 0xEC, 0x0C};
constexpr size_t instruction_buffer_size = sizeof(instruction_buffer) / sizeof(uint8_t);
constexpr size_t instruction_count = 9;
constexpr uint8_t instruction_offsets[] = {0x03, 0x01, 0x03, 0x07, 0x03, 0x08, 0x01, 0x01, 0x04};

/* These arrays of strings are necessary to create the mockUp elf-object & the mockUp arguments-object */
constexpr const char* search_strings[] = {"./resources", "./test/resources", "../resources", "../test/resources",
										  "../../resources", "../../test/resources"};
constexpr size_t search_strings_count = 6;

/* These buffers represent random risc-v instructions.
 * They will be written alongside the instructions into the CodeStore. */
constexpr utils::riscv_instruction_t riscv_buffer[] = {0x12345678, 0xf00dbabe, 0xf0f0f0f0, 0x87654321, 0x18273645,
													   0x90abcdef, 0x01010101, 0x23232323, 0x45454545, 0x67676767,
													   0x89898989, 0xabababab, 0xcdcdcdcd, 0xefefefef};
constexpr size_t riscv_buffer_size = sizeof(riscv_buffer) / sizeof(utils::riscv_instruction_t);

/* These block-addresses are used as offsets into the elf-image, where basic-blocks will be created. These must not be
 * sorted, in order to test, what happens if the CodeStore is fed with random data. */
constexpr uintptr_t x86_block_addresses[] = {0x00000000, 0x000004043, 0x00000889, 0x00008075, 0x00009ff0, 0x00000c06,
											 0x00000ac01, 0x00004806 };
constexpr size_t x86_block_addresses_size = sizeof(x86_block_addresses) / sizeof(uintptr_t);

TEST_CASE("codestore test instruction-adding", "[codestore]") {
	// extract the base-path to for the elf-object
	std::string base_path;
	for (size_t i = 0; i < search_strings_count; i++) {
		std::string temp_path(search_strings[i]);
		temp_path += "/dump_me";
		if (access(temp_path.c_str(), F_OK) != -1) {
			base_path = temp_path;
			break;
		}
	}
	if (base_path.empty())
		FAIL("couldn't find resource-directory");

	// create the elf-object and the arguments-object
	auto temp_elf = elf::Elf(base_path.c_str());

	const char* test_path[] = {"./oxtra", base_path.c_str()};
	auto temp_args = arguments::Arguments(2, const_cast<char**>(test_path));

	// create the code-store object
	CodeStore store = CodeStore(temp_args, temp_elf);

	SECTION("create an empty block") {
		BlockEntry& entry = store.create_block();
		REQUIRE(entry.x86_start == 0);
		REQUIRE(entry.x86_end == 0);
		REQUIRE(entry.instruction_count == 0);
		REQUIRE(entry.offsets == nullptr);
		REQUIRE(entry.riscv_start == 0);
		REQUIRE(&entry != &store.create_block());
	}
	SECTION("add multiple instructions") {
		BlockEntry& entry = store.create_block();
		size_t offset = 0;
		for (size_t index = 0; index < instruction_count; index++) {
			// decode the test-instruction
			fadec::Instruction instruction = fadec::Instruction();
			fadec::decode(instruction_buffer + offset, instruction_buffer_size - offset, fadec::DecodeMode::decode_64,
						  temp_elf.get_base_vaddr() + offset, instruction);

			// add the instruction to the code-store
			REQUIRE_NOTHROW(store.add_instruction(entry, instruction,
												  const_cast<utils::riscv_instruction_t*>(riscv_buffer),
												  riscv_buffer_size - (index & 0x01u) ? 1 : 0));
			offset += instruction.get_size();
		}

		REQUIRE(entry.x86_start == temp_elf.get_base_vaddr());
		REQUIRE(entry.x86_end - entry.x86_start == instruction_buffer_size);
		REQUIRE(entry.instruction_count == instruction_count);
		REQUIRE(entry.riscv_start > 0);

		uintptr_t current_riscv = entry.riscv_start;
		for (size_t index = 0; index < instruction_count; index++) {
			// compare the riscv data
			REQUIRE(entry.offsets[index].riscv == (riscv_buffer_size - (index & 0x01u) ? 1 : 0));
			REQUIRE(memcmp(reinterpret_cast<void*>(current_riscv), riscv_buffer, entry.offsets[index].riscv) == 0);

			// compare the x86-data
			REQUIRE(entry.offsets[index].x86 == instruction_offsets[index]);
		}

		// try adding one instruction, which is out of order
		fadec::Instruction instruction = fadec::Instruction();
		fadec::decode(instruction_buffer, instruction_buffer_size, fadec::DecodeMode::decode_64,
					  temp_elf.get_base_vaddr() + offset - 1, instruction);
	}
	SECTION("add multiple blocks and resolve blocks") {
		/* as long as the default-static_list size recieved form arguments is small enough,
		 * this test also tests reallocation of new static_list-entries */
		REQUIRE(temp_args.get_instruction_list_size() <
				x86_block_addresses_size * instruction_count * riscv_buffer_size);

		// store the starting-offsets of the different pages
		uintptr_t starting_address[x86_block_addresses_size];

		// add multiple blocks to different addresses
		for (size_t i = 0; i < x86_block_addresses_size; i++) {
			BlockEntry& entry = store.create_block();
			size_t offset = 0;
			for (size_t index = 0; index < instruction_count; index++) {
				fadec::Instruction instruction = fadec::Instruction();
				fadec::decode(instruction_buffer + offset, instruction_buffer_size - offset,
							  fadec::DecodeMode::decode_64,
							  temp_elf.get_base_vaddr() + offset + x86_block_addresses[i], instruction);
				REQUIRE_NOTHROW(store.add_instruction(entry, instruction,
													  const_cast<utils::riscv_instruction_t*>(riscv_buffer),
													  riscv_buffer_size));
				if (index + 1 == instruction_count)
					starting_address[i] = entry.riscv_start;
				offset += instruction.get_size();
			}
		}

		// test resolving every address within every block
		for (size_t i = 0; i < x86_block_addresses_size; i++) {
			uintptr_t temp_address = x86_block_addresses[i] + temp_elf.get_base_vaddr();
			for (size_t index = 0; index < instruction_count; index++) {
				REQUIRE_NOTHROW(store.find(temp_address));
				REQUIRE(starting_address[i] + riscv_buffer_size * sizeof(utils::riscv_instruction_t) * index ==
						store.find(temp_address));
				temp_address += instruction_offsets[index];
			}
		}

		/* test to resolve to every next page (in step-sizes of 0x83 bytes
		 * [odd number in order to not be aligned with the block_addresses] ) */
		size_t next_block_index = 0;
		for(uintptr_t addr = 0;; addr += 0x83) {
			if(addr >= x86_block_addresses[next_block_index] + instruction_buffer_size) {
				// find the next block and set the index to the size, if no block has been found
				next_block_index = x86_block_addresses_size;
				for(size_t i = 0; i < x86_block_addresses_size; i++) {
					if(x86_block_addresses[i] >= addr) {
						if(next_block_index == x86_block_addresses_size)
							next_block_index = i;
						else if(x86_block_addresses[i] < x86_block_addresses[next_block_index])
							next_block_index = i;
					}
				}
			}
			BlockEntry* entry = store.get_next_block(addr + temp_elf.get_base_vaddr());
			if(next_block_index >= x86_block_addresses_size) {
				REQUIRE(entry == nullptr);
				break;
			}
			REQUIRE(temp_elf.get_base_vaddr() + x86_block_addresses[next_block_index] == entry->x86_start);
		}
	}
}
