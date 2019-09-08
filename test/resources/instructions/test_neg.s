.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, 0
neg rax
jc exit
jo exit
jnz exit
inc rax
neg rax
jnc exit
jo exit
jz exit
mov rax, 0x8000000000000000
neg rax
jnc exit
jno exit

mov rdi, 2
mov eax, 0
neg eax
jc exit
jo exit
jnz exit
inc eax
neg eax
jnc exit
jo exit
jz exit
mov eax, 0x80000000
neg eax
jnc exit
jno exit

mov rdi, 3
mov ax, 0
neg ax
jc exit
jo exit
jnz exit
inc ax
neg ax
jnc exit
jo exit
jz exit
mov ax, 0x8000
neg ax
jnc exit
jno exit

mov rdi, 4
mov al, 0
neg al
jc exit
jo exit
jnz exit
inc al
neg al
jnc exit
jo exit
jz exit
mov al, 0x80
neg al
jnc exit
jno exit

mov rdi, 5
mov ah, 0
neg ah
jc exit
jo exit
jnz exit
inc ah
neg ah
jnc exit
jo exit
jz exit
mov ah, 0x80
neg ah
jnc exit
jno exit

xor rdi, rdi

exit:
mov rax, 0x3C
syscall
