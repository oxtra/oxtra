#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./create_nolibc.sh <outputfile>"
	exit 1
fi

echo "Enter your assembly code and finish with Ctrl+D"

readarray -t lines

echo "void _start() { asm(" > /tmp/.create_nolibc.tmp.c

for line in "${lines[@]}"; do
	echo "\"$line;\"" >> /tmp/.create_nolibc.tmp.c
done

echo ");}" >> /tmp/.create_nolibc.tmp.c

gcc -static -nostdlib -m64 -masm=intel /tmp/.create_nolibc.tmp.c -o $1

rm /tmp/.create_nolibc.tmp.c
