.intel_syntax noprefix
.global _start
_start:
# add flags integration test binary
mov rdi, 1
mov rax, -1
add rax, 1
jnc exit
jo exit
add rax, -1
jc exit
jo exit
add rax, 0
jc exit
jo exit

mov rdi, 2
mov eax, -1
add eax, 1
jnc exit
jo exit
add eax, -1
jc exit
jo exit
add eax, 0
jc exit
jo exit

mov rdi, 3
mov ax, -1
add ax, 1
jnc exit
jo exit
add ax, -1
jc exit
jo exit
add ax, 0
jc exit
jo exit

mov rdi, 4
mov al, -1
add al, 1
jnc exit
jo exit
add al, -1
jc exit
jo exit
add al, 0
jc exit
jo exit

mov rdi, 5
mov ah, -1
add ah, 1
jnc exit
jo exit
add ah, -1
jc exit
jo exit
add ah, 0
jc exit
jo exit

mov rdi, 6
mov rax, 0x8000000000000000
add rax, -1
jno exit
jnc exit
mov rax, 0x7FFFFFFFFFFFFFFF
add rax, 1
jno exit
jc exit
add rax, 12
jo exit
jc exit

mov rdi, 7
mov eax, 0x80000000
add eax, -1
jno exit
jnc exit
mov eax, 0x7FFFFFFF
add eax, 1
jno exit
jc exit
add eax, 12
jo exit
jc exit

mov rdi, 8
mov ax, 0x8000
add ax, -1
jno exit
jnc exit
mov ax, 0x7FFF
add ax, 1
jno exit
jc exit
add ax, 12
jo exit
jc exit

mov rdi, 9
mov al, 0x80
add al, -1
jno exit
jnc exit
mov al, 0x7F
add al, 1
jno exit
jc exit
add al, 12
jo exit
jc exit

mov rdi, 10
mov ah, 0x80
add ah, -1
jno exit
jnc exit
mov ah, 0x7F
add ah, 1
jno exit
jc exit
add ah, 12
jo exit
jc exit

mov rdi, 0

exit:
mov rax, 0x3C
syscall
