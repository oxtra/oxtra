.intel_syntax noprefix
.global _start
_start:
mov rax, rbx
mov rbx, rcx
push rax
jmp testa

testb:
mov rax, 0x3c
mov rdi, 0xfa

testa:
mov rcx, rdx
jmp testb
