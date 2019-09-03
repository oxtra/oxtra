#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./create_nolibc_c.sh <outputfile>"
	exit 1
fi

echo "Enter your C code and finish with Ctrl+D"
echo "int main() { "

readarray -t lines

echo "int main() { " > $1.c

for line in "${lines[@]}"; do
	echo "	$line" >> $1.c
done

echo "}" >> $1.c

echo "void _start() { register int *ret asm (\"rdi\") = main();" >> $1.c
echo "asm(\"mov rax,60;syscall\");" >> $1.c
echo "}" >> $1.c

gcc -static -nostdlib -m64 -masm=intel $1.c -o $1
