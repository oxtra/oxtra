.intel_syntax noprefix
.global _start
_start:

mov rcx, 0x0123456789ABCD02

mov rdi, 1
mov rax, 0xC000000000000001
rol rax, 66
cmp rax, 7
jne exit

mov rax, 0xC000000000000001
rol rax, cl
cmp rax, 7
jne exit

mov rdi, 2
mov rax, 0x12345678C0000001
rol eax, 34
cmp rax, 7
jne exit

mov rax, 0x12345678C0000001
rol eax, cl
cmp rax, 7
jne exit

mov rdi, 3
mov rax, 0x123456789ABCC001
mov rdx, 0x123456789ABC0007
rol ax, 18
cmp rax, rdx
jne exit

mov rax, 0x123456789ABCC001
mov rdx, 0x123456789ABC0007
rol ax, cl
cmp rax, rdx
jne exit

mov rdi, 4
mov rax, 0x123456789ABCDEC1
mov rdx, 0x123456789ABCDE07
rol al, 10
cmp rax, rdx
jne exit

mov rax, 0x123456789ABCDEC1
mov rdx, 0x123456789ABCDE07
rol al, cl
cmp rax, rdx
jne exit

mov rdi, 5
mov rax, 0x123456789ABCC1EF
mov rdx, 0x123456789ABC07EF
rol ah, 10
cmp rax, rdx
jne exit

mov rax, 0x123456789ABCC1EF
mov rdx, 0x123456789ABC07EF
rol ah, cl
cmp rax, rdx
jne exit

mov rdi, 6
mov rax, 0x8000000000000003
mov rdx, 0xE000000000000000
ror rax, 66
cmp rax, rdx
jne exit

mov rax, 0x8000000000000003
mov rdx, 0xE000000000000000
ror rax, cl
cmp rax, rdx
jne exit

mov rdi, 7
mov rax, 0x1234567880000003
mov rdx, 0x00000000E0000000
ror eax, 34
cmp rax, rdx
jne exit

mov rax, 0x1234567880000003
mov rdx, 0x00000000E0000000
ror eax, cl
cmp rax, rdx
jne exit

mov rdi, 8
mov rax, 0x123456789ABC8003
mov rdx, 0x123456789ABCE000
ror ax, 18
cmp rax, rdx
jne exit

mov rax, 0x123456789ABC8003
mov rdx, 0x123456789ABCE000
ror ax, cl
cmp rax, rdx
jne exit

mov rdi, 9
mov rax, 0x123456789ABCDE83
mov rdx, 0x123456789ABCDEE0
ror al, 10
cmp rax, rdx
jne exit

mov rax, 0x123456789ABCDE83
mov rdx, 0x123456789ABCDEE0
ror al, cl
cmp rax, rdx
jne exit

mov rdi, 10
mov rax, 0x123456789ABC83DE
mov rdx, 0x123456789ABCE0DE
ror ah, 10
cmp rax, rdx
jne exit

mov rax, 0x123456789ABC83DE
mov rdx, 0x123456789ABCE0DE
ror ah, cl
cmp rax, rdx
jne exit

xor rdi, rdi
exit:
mov rax, 60
syscall
