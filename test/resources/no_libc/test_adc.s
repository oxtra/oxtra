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

xor rdi, rdi

exit:
    mov rax, 0x3C
    syscall
