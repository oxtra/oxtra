.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, 1
dec rax
jo exit
dec rax
jo exit
dec rax
jo exit
mov rax, 0x8000000000000001
dec rax
jo exit
dec rax
jno exit
dec rax
jo exit

mov rdi, 2
mov eax, 1
dec eax
jo exit
dec eax
jo exit
dec eax
jo exit
mov eax, 0x80000001
dec eax
jo exit
dec eax
jno exit
dec eax
jo exit

mov rdi, 3
mov ax, 1
dec ax
jo exit
dec ax
jo exit
dec ax
jo exit
mov ax, 0x8001
dec ax
jo exit
dec ax
jno exit
dec ax
jo exit

mov rdi, 4
mov al, 1
dec al
jo exit
dec al
jo exit
dec al
jo exit
mov al, 0x81
dec al
jo exit
dec al
jno exit
dec al
jo exit

mov rdi, 5
mov ah, 1
dec ah
jo exit
dec ah
jo exit
dec ah
jo exit
mov ah, 0x81
dec ah
jo exit
dec ah
jno exit
dec ah
jo exit

xor rdi, rdi
exit:
mov rax, 0x3C
syscall

