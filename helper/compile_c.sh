#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./compile_c.sh <outputfile>"
	echo "The c file has to be in the same directory as the outputfile"
	exit 1
fi

if ! [ -x "$(command -v musl-gcc)" ]; then
	echo "Musl-GCC is not installed"
	echo "Use apt-get install musl-tools to instal it"
	exit 2
fi

musl-gcc -static -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-ssse3 -mno-sse4.1 -mno-sse4.2 -mno-sse4 -mno-avx -mno-avx2 -mno-avx512f -mno-avx512pf -mno-avx512er -mno-avx512cd -mno-sha -mno-aes -mno-pclmul -mno-fsgsbase -mno-rdrnd -mno-f16c -mno-fma -mno-prefetchwt1 -mno-sse4a -mno-fma4 -mno-xop -mno-lwp -mno-3dnow -mno-popcnt -mno-abm -mno-bmi -mno-bmi2 -mno-lzcnt -mno-tbm -m64 -masm=intel $1.c -o $1
