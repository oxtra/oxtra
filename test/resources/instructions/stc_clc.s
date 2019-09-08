.intel_syntax noprefix
.global _start
_start:
mov rdi, 1

stc
jnc exit
clc
jc exit

xor rdi, rdi

exit:
mov rax, 0x3C
syscall
