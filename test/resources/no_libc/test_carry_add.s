.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, -1
add rax, 1
jnc exit
add rax, -1
jc exit
add rax, 0
jc exit

mov rdi, 2
mov eax, -1
add eax, 1
jnc exit
add eax, -1
jc exit
add eax, 0
jc exit

mov rdi, 3
mov ax, -1
add ax, 1
jnc exit
add ax, -1
jc exit
add ax, 0
jc exit

mov rdi, 4
mov al, -1
add al, 1
jnc exit
add al, -1
jc exit
add al, 0
jc exit

mov rdi, 5
mov ah, -1
add ah, 1
jnc exit
add ah, -1
jc exit
add ah, 0
jc exit

mov rdi, 0

exit:
mov rax, 0x3C
syscall

