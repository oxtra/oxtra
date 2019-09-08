.intel_syntax noprefix
.global _start
_start:
# sub flags integration
mov rdi, 1
mov rax, 12
sub rax, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub rax, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 2
mov eax, 12
sub eax, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub eax, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 3
mov ax, 12
sub ax, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub ax, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 4
mov al, 12
sub al, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub al, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 5
mov ah, 12
sub ah, 12 # rax = 0 cf = 0 of = 0
jc exit
jo exit
sub ah, 1 # rax = -1 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 6
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

mov rdi, 7
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

mov rdi, 8
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

mov rdi, 9
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

mov rdi, 10
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

xor rdi, rdi

exit:
mov rax, 0x3C
syscall
