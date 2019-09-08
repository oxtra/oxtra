.intel_syntax noprefix
.global _start
_start:

mov rdi, 1
mov rax, -1 # rax = -1
add rax, 1 # rax = 0 cf = 1
adc rax, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit
adc rax, 0 # rax = 1 cf = 0
jc exit
jo exit
adc rax, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit

mov rdi, 2
mov eax, -1 # rax = -1
add eax, 1 # rax = 0 cf = 1
adc eax, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit
adc eax, 0 # rax = 1 cf = 0
jc exit
jo exit
adc eax, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit

mov rdi, 3
mov ax, -1 # rax = -1
add ax, 1 # rax = 0 cf = 1
adc ax, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit
adc ax, 0 # rax = 1 cf = 0
jc exit
jo exit
adc ax, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit

mov rdi, 4
mov al, -1 # rax = -1
add al, 1 # rax = 0 cf = 1
adc al, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit
adc al, 0 # rax = 1 cf = 0
jc exit
jo exit
adc al, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit

mov rdi, 5
mov ah, -1 # rax = -1
add ah, 1 # rax = 0 cf = 1
adc ah, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit
adc ah, 0 # rax = 1 cf = 0
jc exit
jo exit
adc ah, -1 # rax = 0 cf = 1
jnc exit
jnz exit
jo exit

mov rdi, 6
mov rax, -1
add rax, 1 # rax = 0 cf = 1
mov rax, 0x8000000000000000
adc rax, -1 # rax = -2^63 cf = 1 of = 0
jnc exit
jo exit
adc rax, -2 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
#adc rax, 0 # rax = -2^63 cf = 0 of = 1
#jc exit
#jno exit
#adc rax, -1 # rax = 2^63-1 cf = 1 of = 1
#jnc exit
#jno exit
#adc rax, -12 # rax = 2^63-13 cf = 1 of = 0
#jnc exit
#jo exit

mov rdi, 7
mov eax, -1
add eax, 1 # rax = 0 cf = 1
mov eax, 0x80000000
adc eax, -1 # rax = -2^63 cf = 1 of = 0
jnc exit
jo exit
adc eax, -2 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
adc eax, 0 # rax = -2^63 cf = 0 of = 1
jc exit
jno exit
adc eax, -1 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
adc eax, -12 # rax = 2^63-13 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 8
mov ax, -1
add ax, 1 # rax = 0 cf = 1
mov ax, 0x8000
adc ax, -1 # rax = -2^63 cf = 1 of = 0
jnc exit
jo exit
adc ax, -2 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
adc ax, 0 # rax = -2^63 cf = 0 of = 1
jc exit
jno exit
adc ax, -1 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
adc rax, -12 # rax = 2^63-13 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 9
mov al, -1
add al, 1 # rax = 0 cf = 1
mov al, 0x80
adc al, -1 # rax = -2^63 cf = 1 of = 0
jnc exit
jo exit
adc al, -2 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
adc al, 0 # rax = -2^63 cf = 0 of = 1
jc exit
jno exit
adc al, -1 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
adc al, -12 # rax = 2^63-13 cf = 1 of = 0
jnc exit
jo exit

mov rdi, 10
mov ah, -1
add ah, 1 # rax = 0 cf = 1
mov ah, 0x80
adc ah, -1 # rax = -2^63 cf = 1 of = 0
jnc exit
jo exit
adc ah, -2 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
adc ah, 0 # rax = -2^63 cf = 0 of = 1
jc exit
jno exit
adc ah, -1 # rax = 2^63-1 cf = 1 of = 1
jnc exit
jno exit
adc ah, -12 # rax = 2^63-13 cf = 1 of = 0
jnc exit
jo exit


clc
mov rdi, 11
mov rcx, 12
mov rax, -1
adc rax, 1 # -1 + 1 = 0 cf = 1
jnc exit
test rax, rax
jnz exit
stc
adc rax, -1 # 0 + -1 + 1 = 0 cf = 1
jnc exit
jnz exit
adc rax, rcx # 0 + 12 + 1 = 13 cf = 0
jc exit
cmp rax, 13
jne exit
clc
mov rcx, -37
adc rax, rcx # 13 - 37 = -24 cf = 0
jc exit
cmp rax, -24
jne exit

clc
mov rdi, 12
mov ecx, 12
mov eax, -1
adc eax, 1 # -1 + 1 = 0 cf = 1
jnc exit
test eax, eax
jnz exit
stc
adc eax, -1 # 0 + -1 + 1 = 0 cf = 1
jnc exit
jnz exit
adc eax, ecx # 0 + 12 + 1 = 13 cf = 0
jc exit
cmp eax, 13
jne exit
clc
mov ecx, -37
adc eax, ecx # 13 - 37 = -24 cf = 0
jc exit
cmp eax, -24
jne exit

clc
mov rdi, 13
mov cx, 12
mov ax, -1
adc ax, 1 # -1 + 1 = 0 cf = 1
jnc exit
test ax, ax
jnz exit
stc
adc ax, -1 # 0 + -1 + 1 = 0 cf = 1
jnc exit
jnz exit
adc ax, cx # 0 + 12 + 1 = 13 cf = 0
jc exit
cmp ax, 13
jne exit
clc
mov cx, -37
adc ax, cx # 13 - 37 = -24 cf = 0
jc exit
cmp ax, -24
jne exit

clc
mov rdi, 14
mov cl, 12
mov al, -1
adc al, 1 # -1 + 1 = 0 cf = 1
jnc exit
test al, al
jnz exit
stc
adc al, -1 # 0 + -1 + 1 = 0 cf = 1
jnc exit
jnz exit
adc al, cl # 0 + 12 + 1 = 13 cf = 0
jc exit
cmp al, 13
jne exit
clc
mov cl, -37
adc al, cl # 13 - 37 = -24 cf = 0
jc exit
cmp al, -24
jne exit

clc
mov rdi, 15
mov ch, 12
mov ah, -1
adc ah, 1 # -1 + 1 = 0 cf = 1
jnc exit
test ah, ah
jnz exit
stc
adc ah, -1 # 0 + -1 + 1 = 0 cf = 1
jnc exit
jnz exit
adc ah, ch # 0 + 12 + 1 = 13 cf = 0
jc exit
cmp ah, 13
jne exit
clc
mov ch, -37
adc ah, ch # 13 - 37 = -24 cf = 0
jc exit
cmp ah, -24
jne exit

xor rdi, rdi

exit:
    mov rax, 0x3C
    syscall
