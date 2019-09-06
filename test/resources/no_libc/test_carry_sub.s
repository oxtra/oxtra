.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, 0
sub rax, 1
jnc exit
sub rax, -1
jc exit
sub rax, 0
jc exit

mov rdi, 2
mov eax, 0
sub eax, 1
jnc exit
sub eax, -1
jc exit
sub eax, 0
jc exit

mov rdi, 3
mov ax, 0
sub ax, 1
jnc exit
sub ax, -1
jc exit
sub ax, 0
jc exit

mov rdi, 4
mov al, 0
sub al, 1
jnc exit
sub al, -1
jc exit
sub al, 0
jc exit

mov rdi, 5
mov ah, 0
sub ah, 1
jnc exit
sub ah, -1
jc exit
sub ah, 0
jc exit

mov rdi, 0

exit:
mov rax, 0x3C
syscall
