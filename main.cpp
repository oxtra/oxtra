#include <cstdint>
#include <cstdio>
#include <sys/mman.h>
#include <cstring>
extern "C" {
    #include "../fadec/fadec.h"
}

int readelf(char *file, uint8_t *x86code, int x86size) {
    return 0;
}

int translate(uint8_t *x86code, int x86size, uint8_t *rvcode, int rvsize) {
    // TODO: in decode und encode aufspalten
    FdInstr instr;
    uint8_t *rvcurr = rvcode;
    uint8_t *x86curr = x86code;

    for (;;) {
        int fdret = fd_decode(x86curr, x86size, 64, 0, &instr);
        if (fdret < 0) {
            printf("decode failed: %d\n", fdret);
            return -1;
        }

        if (instr.type >= 259 && instr.type <= 268) { // mov
            // lui a0, imm
            // imm[31:12] rd 0110111
            auto lui = instr.imm & 0xfffff000;
            lui |= 0x537;
            uint8_t *tmp = reinterpret_cast<uint8_t *>(&lui);
            rvcurr[0] = tmp[0];
            rvcurr[1] = tmp[1];
            rvcurr[2] = tmp[2];
            rvcurr[3] = tmp[3];

            rvcurr += 4;

            // ori a0, x0, imm
            // imm[11:0] rs1 110 rd 0010011
            // 10000 110 01010 0010011
            auto ori = instr.imm << 20;
            ori |= 0x6513;
            tmp = reinterpret_cast<uint8_t *>(&ori);
            rvcurr[0] = tmp[0];
            rvcurr[1] = tmp[1];
            rvcurr[2] = tmp[2];
            rvcurr[3] = tmp[3];
        } else if (instr.type == 313) { // ret
            // jalr x0, x1, 0
            // imm[11:0]    rs1   000 rd    1100111
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

        x86curr += instr.size;
        rvcurr += 4;
        if (x86curr - x86code > x86size) {
            printf("exceeded x86 buffer\n");
            return -1;
        } if (rvcurr - rvcode > rvsize) {
            printf("exceeded rv buffer\n");
            return -1;
        }
    }

    return 0;
}

int dispatch(uint8_t *rvcode, int rvsize) {
    // write our code into a block of memory and make it executable
    void *mem = mmap(NULL, rvsize, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
    memcpy(mem, rvcode, rvsize);
    mprotect(mem, rvsize, PROT_READ | PROT_EXEC);

    // change our memory pointer to a function pointer to allow execution
    int (*func)() = reinterpret_cast<int(*)()>(mem);

    printf("%d\n", func());
    return 0;
}

int main(int argc, char **argv) {
    uint8_t x86code[] = {0xb8, 0x02, 0x00, 0x00, 0x00, 0xc3}; // mov eax, 2; ret
    uint8_t rvcode[12] = {0};

    readelf(argv[1], x86code, sizeof(x86code));
    translate(x86code, sizeof(x86code), rvcode, sizeof(rvcode));
    dispatch(rvcode, sizeof(rvcode));

    return 0;
}
