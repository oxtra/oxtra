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

mov rdi, 11
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

mov rdi, 12
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

mov rdi, 13
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

mov rdi, 14
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

mov rdi, 15
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

mov rdi, 16
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

mov rdi, 17
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

mov rdi, 18
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

mov rdi, 19
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

mov rdi, 20
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

mov rdi, 21
mov rax, 0
add rax, 0x800
cmp rax, 0x800
jne exit
add rax, -0x800
test rax, rax
jnz exit
add rax, -0x801
cmp rax, -0x801
jne exit
add rax, 0x801
test rax, rax
jnz exit
add rax, 0x7FF
cmp rax, 0x7FF
jne exit
add rax, -0x7FF
test rax, rax
jnz exit

mov rdi, 22
mov eax, 0
add eax, 0x800
cmp eax, 0x800
jne exit
add eax, -0x800
test eax, eax
jnz exit
add eax, -0x801
cmp eax, -0x801
jne exit
add eax, 0x801
test eax, eax
jnz exit
add eax, 0x7FF
cmp eax, 0x7FF
jne exit
add eax, -0x7FF
test eax, eax
jnz exit

mov rdi, 23
mov ax, 0
add ax, 0x800
cmp ax, 0x800
jne exit
add ax, -0x800
test ax, ax
jnz exit
add ax, -0x801
cmp ax, -0x801
jne exit
add ax, 0x801
test ax, ax
jnz exit
add ax, 0x7FF
cmp ax, 0x7FF
jne exit
add ax, -0x7FF
test ax, ax
jnz exit

mov rdi, 24
mov cl, 25
mov al, 0
add al, cl
cmp al, cl
jne exit
add al, -25
test al, al
jnz exit
mov cl, -16
add al, cl
cmp al, cl
jne exit
add al, 16
test al, al
jnz exit
mov cl, 89
add al, cl
cmp al, cl
jne exit
add al, -89
test al, al
jnz exit

mov rdi, 25
mov cl, 25
mov ah, 0
add ah, cl
cmp ah, cl
jne exit
add ah, -25
test ah, ah
jnz exit
mov cl, -16
add ah, cl
cmp ah, cl
jne exit
add ah, 16
test ah, ah
jnz exit
mov cl, 89
add ah, cl
cmp ah, cl
jne exit
add ah, -89
test ah, ah
jnz exit

xor rdi, rdi

exit:
mov rax, 0x3C
syscall

