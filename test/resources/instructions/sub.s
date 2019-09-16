.intel_syntax noprefix
.global _start
_start:
# sub flags integration
mov rbx, 1
mov rcx, -1
mov rdx, 12
mov rdi, 1
mov rax, 12
sub rax, rdx # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub rax, rbx # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 2
mov eax, 12
sub eax, edx # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub eax, ebx # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 3
mov ax, 12
sub ax, dx # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub ax, bx # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 4
mov al, 12
sub al, dl # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub al, bl # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 5
mov ah, 12
sub ah, dl # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub ah, bl # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 6
mov rax, 0x8000000000000000
sub rax, rbx # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub rax, rcx # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub rax, rbx # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub rax, rdx # cf = 0 of = 0
jc exit
jo exit

mov rdi, 7
mov eax, 0x80000000
sub eax, ebx # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub eax, ecx # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub eax, ebx # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub eax, edx # cf = 0 of = 0
jc exit
jo exit

mov rdi, 8
mov ax, 0x8000
sub ax, bx # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub ax, cx # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub ax, bx # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub ax, dx # cf = 0 of = 0
jc exit
jo exit

mov rdi, 9
mov al, 0x80
sub al, bl # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub al, cl # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub al, bl # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub al, dl # cf = 0 of = 0
jc exit
jo exit

mov rdi, 10
mov ah, 0x80
sub ah, bl # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub ah, cl # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub ah, bl # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub ah, dl # cf = 0 of = 0
jc exit
jo exit

mov rdi, 11
mov rax, 12
sub rax, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub rax, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 12
mov eax, 12
sub eax, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub eax, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 13
mov ax, 12
sub ax, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub ax, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 14
mov al, 12
sub al, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub al, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 15
mov ah, 12
sub ah, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub ah, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 16
mov rax, 0x8000000000000000
sub rax, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub rax, -1 # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub rax, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub rax, 12 # cf = 0 of = 0
jc exit
jo exit

mov rdi, 17
mov eax, 0x80000000
sub eax, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub eax, -1 # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub eax, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub eax, 12 # cf = 0 of = 0
jc exit
jo exit

mov rdi, 18
mov ax, 0x8000
sub ax, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub ax, -1 # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub ax, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub ax, 12 # cf = 0 of = 0
jc exit
jo exit

mov rdi, 19
mov al, 0x80
sub al, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub al, -1 # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub al, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub al, 12 # cf = 0 of = 0
jc exit
jo exit

mov rdi, 20
mov ah, 0x80
sub ah, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub ah, -1 # rax = -2^63 cf = 1 of = 1
jnc exit
jno exit
sub ah, 1 # rax = 2^63-1 cf = 0 of = 1
jc exit
jno exit
sub ah, 12 # cf = 0 of = 0
jc exit
jo exit

mov rdi, 21
xor rax, rax
sub rax, -0x800
cmp rax, 0x800
jne exit
sub rax, 0x800
jnz exit
sub rax, -0x7FF
cmp rax, 0x7FF
jne exit
sub rax, 0x7FF
jnz exit
sub rax, 0x9294834
cmp rax, -0x9294834
jne exit

mov rdi, 22
xor eax, eax
sub eax, -0x800
cmp eax, 0x800
jne exit
sub eax, 0x800
jnz exit
sub eax, -0x7FF
cmp eax, 0x7FF
jne exit
sub eax, 0x7FF
jnz exit
sub eax, 0x9294834
cmp eax, -0x9294834
jne exit

mov rdi, 23
xor ax, ax
sub ax, -0x800
cmp ax, 0x800
jne exit
sub ax, 0x800
jnz exit
sub ax, -0x7FF
cmp ax, 0x7FF
jne exit
sub ax, 0x7FF
jnz exit
sub ax, 0x9294
cmp ax, -0x9294
jne exit

mov rdi, 24
xor al, al
sub al, -0xF3
cmp al, 0xF3
jne exit
sub al, 0xF3
jnz exit
sub al, -0x7F
cmp al, 0x7F
jne exit
sub al, 0x7F
jnz exit
sub al, 0x92
cmp al, -0x92
jne exit

mov rdi, 25
xor ah, ah
sub ah, -0xF3
cmp ah, 0xF3
jne exit
sub ah, 0xF3
jnz exit
sub ah, -0x7F
cmp ah, 0x7F
jne exit
sub ah, 0x7F
jnz exit
sub ah, 0x92
cmp ah, -0x92
jne exit

xor rdi, rdi

exit:
mov rax, 0x3C
syscall
