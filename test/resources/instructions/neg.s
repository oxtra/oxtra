.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, 0
neg rax
jc exit
jo exit
jnz exit
cmp rax, 0
jne exit
inc rax
neg rax
jnc exit
jo exit
jz exit
cmp rax, -1
jne exit
mov rcx, 0x8000000000000000
mov rax, rcx
neg rax
jnc exit
jno exit
cmp rax, rcx
jne exit

mov rdi, 2
mov eax, 0
neg eax
jc exit
jo exit
jnz exit
cmp eax, 0
jne exit
inc eax
neg eax
jnc exit
jo exit
jz exit
cmp eax, -1
jne exit
mov ecx, 0x80000000
mov eax, ecx
neg eax
jnc exit
jno exit
cmp eax, ecx
jne exit

mov rdi, 3
mov ax, 0
neg ax
jc exit
jo exit
jnz exit
cmp ax, 0
jne exit
inc ax
neg ax
jnc exit
jo exit
jz exit
cmp ax, -1
jne exit
mov ax, 0x8000
neg ax
jnc exit
jno exit
cmp ax, 0x8000
jne exit

mov rdi, 4
mov al, 0
neg al
jc exit
jo exit
jnz exit
cmp al, 0
jne exit
inc al
neg al
jnc exit
jo exit
jz exit
cmp al, -1
jne exit
mov al, 0x80
neg al
jnc exit
jno exit
cmp al, 0x80
jne exit

mov rdi, 5
mov ah, 0
neg ah
jc exit
jo exit
jnz exit
cmp ah, 0
jne exit
inc ah
neg ah
jnc exit
jo exit
jz exit
cmp ah, -1
jne exit
mov ah, 0x80
neg ah
jnc exit
jno exit
cmp ah, 0x80
jne exit

xor rdi, rdi

exit:
mov rax, 0x3C
syscall
