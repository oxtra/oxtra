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

xor rdi, rdi

exit:
mov rax, 0x3C
syscall
