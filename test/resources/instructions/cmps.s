.intel_syntax noprefix
.global _start
_start:

mov rbx, 1
mov rdi, offset str1
mov rsi, offset str2
cmpsb
jnz exit
jc exit
jo exit
js exit
mov rcx, 7
repz cmpsb
test rcx, rcx
jnz exit
jc exit
jo exit
js exit

mov rcx, 8
repz cmpsb
jz exit
jnc exit
jo exit
jns exit
cmp rcx, 5
jne exit

mov rbx, 2
mov rdi, offset str1
mov rsi, offset str2
cmpsw
jnz exit
jc exit
jo exit
js exit
mov rcx, 3
repz cmpsw
test rcx, rcx
jnz exit
jc exit
jo exit
js exit

mov rbx, 3
mov rdi, offset str1
mov rsi, offset str2
cmpsd
jnz exit
jc exit
jo exit
js exit
mov rcx, 1
repz cmpsd
test rcx, rcx
jnz exit
jc exit
jo exit
js exit

mov rbx, 4
mov rdi, offset str1
mov rsi, offset str2
cmpsq
jnz exit
jc exit
jo exit
js exit
mov rcx, 1
repz cmpsq
test rcx, rcx
jnz exit
jc exit
jo exit
js exit

xor rbx, rbx
exit:
mov rdi, rbx
mov rax, 0x3C
syscall

.section .data
str1: .string "asdfjkl"
str2: .string "asdfjkl"
str3: .string "asaijfe"
