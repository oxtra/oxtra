.intel_syntax noprefix
.global _start
_start:

mov rsi, offset src
mov rdi, offset dst
movsb
mov rcx, 7
rep movsb
mov rdx, dst
cmp rdx, src
mov rdi, 1
jne exit

mov rsi, offset src
mov rdi, offset dst
movsw
mov rcx, 3
rep movsw
mov rdx, dst
cmp rdx, src
mov rdi, 2
jne exit

mov rsi, offset src
mov rdi, offset dst
movsd
mov rcx, 1
rep movsd
mov rdx, dst
cmp rdx, src
mov rdi, 3
jne exit

mov rsi, offset src
mov rdi, offset dst
movsq
mov rdx, dst
cmp rdx, src
mov rdi, 2
jne exit
mov rsi, offset src
mov rdi, offset dst
mov rcx, 1
rep movsq
mov rdx, dst
cmp rdx, src
mov rdi, 2
jne exit

xor rdi, rdi
exit:
mov rax, 0x3C
syscall

.section .data
src: .quad 0x0123456789ABCDEF
dst: .quad 0
