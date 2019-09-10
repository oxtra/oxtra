.intel_syntax noprefix
.global _start
_start:

mov rdi, 1
mov rsi, offset data
lodsb
cmp al, 0x01
jne exit
mov rcx, 7
rep lodsb
cmp al, 0xEF
jne exit

mov rdi, 2
mov rsi, offset data
lodsw
cmp ax, 0x2301
jne exit
mov rcx, 3
rep lodsw
cmp ax, 0xEFCD
jne exit

mov rdi, 3
mov rsi, offset data
lodsd
cmp eax, 0x67452301
jne exit
mov rcx, 1
rep lodsd
cmp eax, 0xEFCDAB89
jne exit

mov rdi, 4
mov rsi, offset data
lodsq
mov rdx, 0xEFCDAB8967452301
cmp rax, rdx
jne exit
mov rcx, 1
mov rsi, offset data
rep lodsq
cmp rax, rdx
jne exit
mov rax, 157
rep lodsq
cmp rax, 157
jne exit

xor rdi, rdi
exit:
mov rax, 0x3C
syscall

.section .data
data: .byte 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF
