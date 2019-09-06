.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, -1 # rax = -1 
add rax, 1 # rax = 0 cf = 1
adc rax, -1 # rax = 0 cf = 1
jnc exit
jnz exit
adc rax, 0 # rax = 1 cf = 0
jc exit
adc rax, -1 # rax = 0 cf = 1
jnc exit
jnz exit

xor rdi, rdi

exit:
    mov rax, 0x3C
    syscall
