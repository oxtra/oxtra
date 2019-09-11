.intel_syntax noprefix
.global _start
_start:
mov rax, 0x0123456789abcdef
mov eax, 0x11223344
mov ax, 0xffff
mov ah, 0xaa
mov al, 0xbb
mov rdi, rax
mov rax, 0x3c
syscall
