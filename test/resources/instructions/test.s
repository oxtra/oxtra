.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
xor rax, rax
test rax, 0
jnz exit
test rax, rax
jnz exit
mov rax, 13
test rax, 13
jz exit
test rax, rax
jz exit

mov rdi, 2
xor eax, eax
test eax, 0
jnz exit
test eax, eax
jnz exit
mov eax, 13
test eax, 13
jz exit
test eax, eax
jz exit

mov rdi, 3
xor ax, ax
test ax, 0
jnz exit
test ax, ax
jnz exit
mov rx, 13
test ax, 13
jz exit
test ax, ax
jz exit

mov rdi, 4
xor al, al
test al, 0
jnz exit
test al, al
jnz exit
mov al, 13
test al, 13
jz exit
test al, al
jz exit

mov rdi, 5
xor ah, ah
test ah, 0
jnz exit
test ah, ah
jnz exit
mov ah, 13
test ah, 13
jz exit
test ah, ah
jz exit

xor rdi, rdi

exit:
mov rax, 0x3C
syscall
