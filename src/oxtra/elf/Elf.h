#ifndef OXTRA_ELF_H
#define OXTRA_ELF_H

extern "C" {
	#include <fadec.h>
}

int read_elf(const char *file, const uint8_t *x86code, int x86size);

#endif //OXTRA_ELF_H
