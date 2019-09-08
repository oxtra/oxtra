.intel_syntax noprefix
.global _start
_start:
# add flags integration test binary
mov rbx, 1
mov rcx, -1
mov rdx, 0
mov rdi, 1
mov rax, -1
add rax, rbx
jnc exit
jo exit
add rax, rcx
jc exit
jo exit
add rax, rdx
jc exit
jo exit

mov rdi, 2
mov eax, ecx
add eax, ebx
jnc exit
jo exit
add eax, ecx
jc exit
jo exit
add eax, edx
jc exit
jo exit

mov rdi, 3
mov ax, cx
add ax, bx
jnc exit
jo exit
add ax, cx
jc exit
jo exit
add ax, dx
jc exit
jo exit

mov rdi, 4
mov al, cl
add al, bl
jnc exit
jo exit
add al, cl
jc exit
jo exit
add al, dl
jc exit
jo exit

mov rdi, 5
mov ah, cl
add ah, bl
jnc exit
jo exit
add ah, cl
jc exit
jo exit
add ah, dl
jc exit
jo exit

mov rdx, 12

mov rdi, 6
mov rax, 0x8000000000000000
add rax, rcx
jno exit
jnc exit
mov rax, 0x7FFFFFFFFFFFFFFF
add rax, rbx
jno exit
jc exit
add rax, rdx
jo exit
jc exit

mov rdi, 7
mov eax, 0x80000000
add eax, ecx
jno exit
jnc exit
mov eax, 0x7FFFFFFF
add eax, ebx
jno exit
jc exit
add eax, edx
jo exit
jc exit

mov rdi, 8
mov ax, 0x8000
add ax, cx
jno exit
jnc exit
mov ax, 0x7FFF
add ax, bx
jno exit
jc exit
add ax, dx
jo exit
jc exit

mov rdi, 9
mov al, 0x80
add al, cl
jno exit
jnc exit
mov al, 0x7F
add al, bl
jno exit
jc exit
add al, dl
jo exit
jc exit

mov rdi, 10
mov ah, 0x80
add ah, cl
jno exit
jnc exit
mov ah, 0x7F
add ah, bl
jno exit
jc exit
add ah, dl
jo exit
jc exit

mov rdi, 0

exit:
mov rax, 0x3C
syscall

