.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, -1
inc rax
jo exit
inc rax
jo exit
mov rax, 0x7FFFFFFFFFFFFFFF
inc rax
jno exit

mov rdi, 2
mov eax, -1
inc eax
jo exit
inc eax
jo exit
mov eax, 0x7FFFFFFF
inc eax
jno exit

mov rdi, 3
mov ax, -1
inc ax
jo exit
inc ax
jo exit
mov ax, 0x7FFF
inc ax
jno exit

mov rdi, 4
mov al, -1
inc al
jo exit
inc al
jo exit
mov al, 0x7F
inc al
jno exit

mov rdi, 5
mov ah, -1
inc ah
jo exit
inc ah
jo exit
mov ah, 0x7F
inc ah
jno exit


xor rdi, rdi

exit:
mov rax, 0x3C
syscall
