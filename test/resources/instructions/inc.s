.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, -1
inc rax
jo exit
inc rax
jo exit
cmp rax, 1
jne exit
mov rax, 0x7FFFFFFFFFFFFFFF
inc rax
jno exit
mov rcx, 0x8000000000000000
cmp rax, rcx
jne exit

mov rdi, 2
mov eax, -1
inc eax
jo exit
inc eax
jo exit
cmp eax, 1
jne exit
mov eax, 0x7FFFFFFF
inc eax
jno exit
cmp eax, 0x80000000
jne exit

mov rdi, 3
mov ax, -1
inc ax
jo exit
inc ax
jo exit
cmp ax, 1
jne exit
mov ax, 0x7FFF
inc ax
jno exit
cmp ax, 0x8000
jne exit

mov rdi, 4
mov al, -1
inc al
jo exit
inc al
jo exit
cmp al, 1
jne exit
mov al, 0x7F
inc al
jno exit
cmp al, 0x80
jne exit

mov rdi, 5
mov ah, -1
inc ah
jo exit
inc ah
jo exit
cmp ah, 1
jne exit
mov ah, 0x7F
inc ah
jno exit
cmp ah, 0x80
jne exit


xor rdi, rdi

exit:
mov rax, 0x3C
syscall
