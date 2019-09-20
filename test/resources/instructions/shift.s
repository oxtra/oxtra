.intel_syntax noprefix
.global _start
_start:

mov rdi, 1
mov rax, 1
mov cl, 66
shl rax, cl
cmp rax, 4
jne exit
shl rax, 66
cmp rax, 16
jne exit

mov rdi, 2
mov rax, 0x89abcdef00000001
mov cl, 34
shl eax, cl
cmp rax, 4
jne exit
shl eax, 2
cmp rax, 16
jne exit

mov rdi, 3
mov rax, 0x456789abcdef0001
mov cl, 2
shl ax, cl
mov rcx, 0x456789abcdef0004
cmp rax, rcx
jne exit
shl ax, 2
mov rcx, 0x456789abcdef0010
cmp rax, rcx
jne exit

mov rdi, 4
mov rax, 0x23456789abcdef01
mov cl, 2
shl al, cl
mov rcx, 0x23456789abcdef04
cmp rax, rcx
jne exit
shl al, 2
mov rcx, 0x23456789abcdef10
cmp rax, rcx
jne exit

mov rdi, 5
mov rax, 0x23456789abcd01ef
mov cl, 2
shl ah, cl
mov rcx, 0x23456789abcd04ef
cmp rax, rcx
jne exit
shl ah, 2
mov rcx, 0x23456789abcd10ef
cmp rax, rcx
jne exit

xor rdi, rdi
exit:
mov rax, 60
syscall
