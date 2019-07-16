#!/bin/bash

if [ $# -ne 1 ]; then
	echo "Usage: ./create_nolibc.sh <outputfile>"
	exit 1
fi

echo "Enter your assembly code and finish with Ctrl+D"

readarray -t lines

echo ".intel_syntax noprefix" > $1.s
echo ".global _start" >> $1.s
echo "_start:" >> $1.s

for line in "${lines[@]}"; do
	echo "$line" >> $1.s
done

gcc -static -nostdlib -m64 -masm=intel $1.s -o $1 && chmod +x $1

objdump -d -M intel $1