.intel_syntax noprefix
.global _start
_start:

mov rdi, 1
mov rax, 0x100000000
mov rcx, rax
bt rax, 32
jnc exit
cmp rax, rcx
jne exit
bt rax, 96
jnc exit
cmp rax, rcx
jne exit

btr rax, 32
jnc exit
cmp rax, 0
jne exit
bts rax, 32
jc exit
cmp rax, rcx
jne exit
btr rax, 96
jnc exit
cmp rax, 0
jne exit
bts rax, 96
jc exit
cmp rax, rcx
jne exit
btc rax, 32
jnc exit
cmp rax, 0
jne exit
btc rax, 96
jc exit
cmp rax, rcx
jne exit

mov rdi, 2
mov eax, 0x10000
mov ecx, eax
bt eax, 16
jnc exit
cmp eax, ecx
jne exit
bt eax, 48
jnc exit
cmp eax, ecx
jne exit

btr eax, 16
jnc exit
cmp eax, 0
jne exit
bts eax, 16
jc exit
cmp eax, ecx
jne exit
btr eax, 48
jnc exit
cmp eax, 0
jne exit
bts eax, 48
jc exit
cmp eax, ecx
jne exit
btc eax, 16
jnc exit
cmp eax, 0
jne exit
btc eax, 48
jc exit
cmp eax, ecx
jne exit

mov rdi, 3
mov ax, 0x100
mov cx, ax
bt ax, 8
jnc exit
cmp ax, cx
jne exit
bt ax, 24
jnc exit
cmp ax, cx
jne exit

btr ax, 8
jnc exit
cmp ax, 0
jne exit
bts ax, 8
jc exit
cmp ax, cx
jne exit
btr ax, 24
jnc exit
cmp ax, 0
jne exit
bts ax, 24
jc exit
cmp ax, cx
jne exit
btc ax, 8
jnc exit
cmp ax, 0
jne exit
btc ax, 24
jc exit
cmp ax, cx
jne exit

xor rdi, rdi
exit:
mov rax, 60
syscall

.data
mem: .8byte 0xAAAAAAAAAAAAAAAA
