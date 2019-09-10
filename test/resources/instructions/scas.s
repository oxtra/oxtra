.intel_syntax noprefix
.global _start
_start:

mov rbx, 1
mov rdi, offset data
mov al, 0xEE
scasb
jnc exit
jo exit
mov al, 0x67
mov rcx, 7
repnz scasb
jnz exit
js exit
jc exit
jo exit
cmp rcx, 3
jne exit

mov rbx, 2
mov rdi, offset data
mov ax, 0xCFEE
scasw
jc exit
jo exit
mov ax, 0x4567
mov rcx, 3
repnz scasw
jnz exit
js exit
jc exit
jo exit
cmp rcx, 1
jne exit

mov rbx, 3
mov rdi, offset data
mov eax, 0x8238CFEE
scasd
jnc exit
jo exit
mov eax, 0x18372857
mov rcx, 1
repnz scasd
jz exit
cmp rcx, 0
jne exit

mov rbx, 4
mov rdi, offset data
mov rax, 0xC1285628B8238CFEE
scasq
jc exit
jo exit
mov rax, 0x18372857
mov rcx, 0
repnz scasd
jz exit
cmp rcx, 0
jne exit
mov rax, 0x3957284692BA8AD3
mov rcx, 3
repnz
test rcx, rcx
jnz exit

xor rbx, rbx
exit:
mov rdi, rbx
mov rax, 0x3C
syscall

.section .data
data: .quad 0x0123456789ABCDEF, 0x2837462958374729, 0x3957284692BA8AD3
