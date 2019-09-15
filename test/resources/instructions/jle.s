.intel_syntax noprefix
.global _start
_start:
mov rdi, 0

mov rax, 2
cmp rax, 0xf423f
jle .end_less
mov rdi, 1
.end_less:

mov rax, 0xf423f
cmp rax, 0xf423f
jle .end_equal
mov rdi, 2
.end_equal:

mov rax, 60
syscall
