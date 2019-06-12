#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/mman.h>

#include "oxtra/elf/Elf.h"

int decode(const uint8_t *x86code, size_t x86size, x86::Instruction *intermediate, size_t inter_size) {
	x86::Instruction instr;
	const uint8_t *x86curr = x86code;
	x86::Instruction *inter_curr = intermediate;

	for (;;) {
		int fdret = x86::decode(x86curr, x86size, x86::DecodeMode::decode_64, 0, instr);
		if (fdret < 0) {
			printf("decode failed: %d\n", fdret);
			return -1;
		}

		*inter_curr = instr;

		++inter_curr;
		x86curr += instr.get_size();

		if (instr.get_type() == x86::InstructionType::RET) break; //ret

		if (x86curr - x86code > x86size) {
			printf("exceeded x86 buffer\n");
			return -1;
		} if (inter_curr - intermediate > inter_size) {
			printf("exceeded intermediate buffer\n");
			return -1;
		}
	}

	return 0;
}

int optimize(x86::Instruction *intermediate, size_t inter_size) {
	return 0;
}

int encode(const x86::Instruction *intermediate, size_t inter_size, uint8_t *rvcode, size_t rvsize) {
	const x86::Instruction *inter_curr = intermediate;
	uint8_t *rvcurr = rvcode;

	for (;;) {
		if (static_cast<uint16_t>(inter_curr->get_type()) >= 259 && static_cast<uint16_t>(inter_curr->get_type()) <= 268) { // mov
			// lui a0, imm
			// imm[31:12] rd 0110111
			uint32_t lui = inter_curr->get_immediate() & 0xfffff000u;
			lui |= 0x537u;
			auto *tmp = reinterpret_cast<uint8_t *>(&lui);
			rvcurr[0] = tmp[0];
			rvcurr[1] = tmp[1];
			rvcurr[2] = tmp[2];
			rvcurr[3] = tmp[3];

			rvcurr += 4;

			// ori a0, x0, imm
			// imm[11:0] rs1 110 rd 0010011
			// 10000 110 01010 0010011
			uint32_t ori = inter_curr->get_immediate() << 20;
			ori |= 0x6513u;
			tmp = reinterpret_cast<uint8_t *>(&ori);
			rvcurr[0] = tmp[0];
			rvcurr[1] = tmp[1];
			rvcurr[2] = tmp[2];
			rvcurr[3] = tmp[3];
		} else if (inter_curr->get_type() == x86::InstructionType::RET) { // ret
			// jalr x0, x1, 0
			// imm[11:0]	rs1   000 rd	1100111
			// 000000000000 00001 000 00000 1100111 
			// 0x0 0x0 0x80 0x67
			rvcurr[0] = 0x67;
			rvcurr[1] = 0x80;
			rvcurr[2] = 0x00;
			rvcurr[3] = 0x00;
			break;
		} else {
			printf("instruction not supported\n");
			return -1;
		}

		++inter_curr;
		rvcurr += 4;

		if (inter_curr - intermediate > inter_size) {
			printf("exceeded x86 buffer\n");
			return -1;
		} if (rvcurr - rvcode > rvsize) {
			printf("exceeded rv buffer\n");
			return -1;
		}
	}

	return 0;
}

int translate(const uint8_t *x86code, size_t x86size, uint8_t *rvcode, size_t rvsize) {
	x86::Instruction intermediate[8];
	decode(x86code, x86size, intermediate, sizeof(intermediate));
	optimize(intermediate, sizeof(intermediate));
	encode(intermediate, sizeof(intermediate), rvcode, rvsize);
	return 0;
}

int dispatch(const uint8_t *rvcode, size_t rvsize) {
	// write our code into a block of memory and make it executable
	void *mem = mmap(nullptr, rvsize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	memcpy(mem, rvcode, rvsize);
	mprotect(mem, rvsize, PROT_READ | PROT_EXEC);

	// change our memory pointer to a function pointer to allow execution
	auto (*func)() = reinterpret_cast<int(*)()>(mem);

	printf("%d\n", func());
	return 0;
}

int main(int argc, char **argv) {
	uint8_t x86code[] = {0xb8, 0x02, 0x00, 0x00, 0x00, 0xc3}; // mov eax, 2; ret
	uint8_t rvcode[12] = {0};

	for (auto cur_instr = x86code; cur_instr < x86code + sizeof(x86code);) {
		x86::Instruction instr;
		if (x86::decode(cur_instr, x86code + sizeof(x86code) - cur_instr, x86::DecodeMode::decode_64, 0x1000, instr) < 0) {
			printf("decoding failed.\n");
			return 0;
		}

		char buffer[512];
		x86::format(instr, buffer, sizeof(buffer));
		printf("%s\n", buffer);

		cur_instr += instr.get_size();
	}

	read_elf(argv[1], x86code, sizeof(x86code));
	translate(x86code, sizeof(x86code), rvcode, sizeof(rvcode));
	dispatch(rvcode, sizeof(rvcode));

	return 0;
}