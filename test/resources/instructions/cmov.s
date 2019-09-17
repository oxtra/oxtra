.intel_syntax noprefix
.global _start
_start:

# clear rax, rcx, cf, of, sf
# set zf, pf
xor rax, rax
xor rcx, rcx

# perform some basic tests that are common to all cmovcc instructions
mov rdi, 1

mov rsi, offset array1
cmovz rax, [rsi]
cmp rax, 1
jne exit

add rax, rax
cmovz rax, [rsi + rax * 8]
cmp rax, 2
jne exit

cmovz rax, [rsi + rax * 8 + 8]
cmp rax, 4
jne exit

cmovz rax, [rsi + 16]
cmp rax, 3
jne exit

mov rsi, offset array2
cmovz eax, [rsi]
cmp eax, 1
jne exit

add eax, eax
cmovz eax, [rsi + rax * 4]
cmp eax, 2
jne exit

cmovz eax, [rsi + rax * 4 + 4]
cmp eax, 4
jne exit

cmovz eax, [rsi + 8]
cmp eax, 3
jne exit

mov rsi, offset array3
cmovz ax, [rsi]
cmp ax, 1
jne exit

add ax, ax
cmovz ax, [rsi + rax * 2]
cmp ax, 2
jne exit

cmovz ax, [rsi + rax * 2 + 2]
cmp ax, 4
jne exit

cmovz ax, [rsi + 4]
cmp ax, 3
jne exit

cmovz rax, rcx
cmp rax, 0
jne exit

cmovnz rax, [rsi]
cmp rax, 0
jne exit

mov rax, 0x123456789abcdef
cmovnz eax, ecx
cmp eax, 0x89abcdef
jne exit
shr rax, 32
cmp eax, 0
jne exit

mov rax, 0x123456789abcdef
cmovz eax, ecx
cmp rax, 0
jne exit

mov rax, 0x123456789abcdef
mov rdx, 0x123456789ab0000
cmovz ax, cx
cmp rax, rdx
jne exit

mov rax, 0x123456789abcdef
mov rdx, 0x123456789abcdef
cmovnz ax, cx
cmp rax, rdx
jne exit

mov rdi, 2
mov rax, 1
cmp rax, 0
cmovna rax, rcx
cmp rax, 1
jne exit
cmp rax, 0
cmova rax, rcx
cmp rax, 0
jne exit

mov rdi, 3
mov rax, 1
cmp rax, 1
cmovnbe rax, rcx
cmp rax, 1
jne exit
cmp rax, 1
cmovbe rax, rcx
cmp rax, 0
jne exit
mov rax, 1
cmp rax, 2
cmovnbe rax, rcx
cmp rax, 1
jne exit
cmp rax, 2
cmovbe rax, rcx
cmp rax, 0
jne exit

mov rdi, 4
stc
mov rax, 1
cmovnc rax, rcx
cmp rax, 1
jne exit
stc
cmovc rax, rcx
cmp rax, 0
jne exit

mov rdi, 5
mov rax, 1
cmp rax, -1
cmovng rax, rcx
cmp rax, 1
jne exit
cmp rax, -1
cmovg rax, rcx
cmp rax, 0
jne exit

mov rdi, 6
mov rax, -1
cmp rax, -1
cmovnge rax, rcx
cmp rax, -1
jne exit
cmp rax, -1
cmovge rax, rcx
cmp rax, 0
jne exit
cmp rax, -1
cmovnge rax, rcx
cmp rax, 0
jne exit
cmp rax, -1
cmovge rax, rcx
cmp rax, 0
jne exit

mov rdi, 7
mov rax, 1
cmp rax, 2
cmovnl
cmp rax, 1
jne exit
cmp rax, 2
cmovl rax, rcx
cmp rax, 0
jne exit

mov rdi, 8
mov rax, 1
cmp rax, 2
cmovnle
cmp rax, 1
jne exit
cmp rax, 2
cmovle rax, rcx
cmp rax, 0
jne exit
mov rax, 1
cmp rax, 1
cmovnle
cmp rax, 1
jne exit
cmp rax, 1
cmovle rax, rcx
cmp rax, 0
jne exit

mov rdi, 9
clc
mov rax, 1
cmovc rax, rcx
cmp rax, 1
jne exit
cmovnc rax, rcx
cmp rax, 0
jne exit

xor rdi, rdi

exit:
mov rax, 60 # exit
syscall


.data
array1: .8byte 1, 2, 3, 4
array2: .4byte 1, 2, 3, 4
array3: .word 1, 2, 3, 4
