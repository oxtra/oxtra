#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./create_nolibc_c.sh <outputfile>"
	exit 1
fi

syscall=$'int _syscall(int number, int param1) {
	int ret;

	asm(\"mov eax, %1;\"
	\"mov edi, %2;\"
	\"syscall;\"
	\"mov %0, eax;\"
	: \"=r\" (ret)
	: \"r\" (number), \"r\" (param1)
	);
						
	return ret;
}\n'

echo "Enter your C code and finish with Ctrl+D"

echo "$syscall"

echo "int main() { "

readarray -t lines

echo "}"

echo "$syscall" > $1.c

echo "int main() { " >> $1.c

for line in "${lines[@]}"; do
	echo "	$line" >> $1.c
done

echo $'}\n' >> $1.c

echo "void _start() { " >> $1.c
echo "	_syscall(60, main());" >> $1.c
echo "}" >> $1.c

gcc -static -nostdlib -m64 -masm=intel $1.c -o $1
