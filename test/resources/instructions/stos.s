.intel_syntax noprefix
.global _start
_start:

test_1:
mov rdi, offset str
mov al, 0x61
stosb
mov rcx, 7
rep stosb
mov rdi, 1
call loop

mov rdi, offset str
mov ax, 0x6161
stosb
mov rcx, 3
rep stosb
mov rdi, 2
call loop

mov rdi, offset str
mov eax, 0x61616161
stosb
mov rcx, 1
rep stosb
mov rdi, 3
call loop

mov rdi, offset str
mov rax, 0x6161616161616161
stosb
mov rcx, 0
rep stosb
mov rdi, 3
call loop

mov rdi, offset str
mov rax, 0x6161616161616161
stosb
mov rax, 0x123827364
mov rcx, 0
rep stosb
mov rdi, 3
call loop

xor rdi, rdi
exit:
mov rax, 0x3C
syscall

loop:
mov dl, [offset str + rcx]
cmp dl, 'a'
jne exit
inc rcx
cmp rcx, 4
jb loop
ret

.section .data
str: .string "00000000"
