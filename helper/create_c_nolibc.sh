#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./create_nolibc_c.sh <outputfile>"
	exit 1
fi

syscall=$'long _syscall(long number, long param1, long param2, long param3) {
	long ret;
	
	asm("mov rax, %1;"
	"mov rdi, %2;"
	"mov rdx, %4;"
	"mov rsi, %3;"
	"syscall;"
	"mov %0, rax;"
	: "=r" (ret)
	: "r" (number), "r" (param1), "r" (param2), "r" (param3)
	);
				
        return ret;
}\n'

write=$'void _write (int fd, const void *buf, long count) {
	_syscall(1, fd, buf, count);
}\n'

print=$'void print(const void *buf) {
	if (buf == 0) return;
	int length;
	for (length = 0; ((char*) buf)[length] != \'\\0\'; length++);
	_write(1, buf, length);
}\n'

read=$'long read(int fd, void *buf, long count) {
	return _syscall(0, fd, buf, count);
}\n'

echo "Enter your C code and finish with Ctrl+D"
echo $'The main():int method will be called\n'

echo "long _syscall(long number, long param1, long param2, long param3);"
echo "void _write (int fd, const void *buf, long count);"
echo "void print(const void *buf);"
echo "long read(int fd, void *buf, long count);"

echo ""

readarray -t lines

echo "$syscall" > $1.c
echo "$write" >> $1.c
echo "$print" >> $1.c
echo "$read" >> $1.c

for line in "${lines[@]}"; do
	echo "$line" >> $1.c
done

echo "void _start() { " >> $1.c
echo "	_syscall(60, main(), 0, 0);" >> $1.c
echo "}" >> $1.c

gcc -static -nostdlib -m64 -masm=intel $1.c -o $1
