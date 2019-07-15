#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./create_nolibc.sh <outputfile>"
	exit 1
fi

echo "Enter your assembly code and finish with Ctrl+D"

readarray -t lines

echo ".intel_syntax noprefix" > /tmp/.create_nolibc.tmp.s
echo ".global _start" >> /tmp/.create_nolibc.tmp.s
echo "_start:" >> /tmp/.create_nolibc.tmp.s

for line in "${lines[@]}"; do
	echo "$line" >> /tmp/.create_nolibc.tmp.s
done

gcc -static -nostdlib -m64 -masm=intel /tmp/.create_nolibc.tmp.s -o $1

rm /tmp/.create_nolibc.tmp.s

objdump -d -M intel $1