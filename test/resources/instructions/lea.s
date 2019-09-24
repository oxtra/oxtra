.intel_syntax noprefix
.global _start
_start:

mov rdi, 1

mov rcx, 2

lea rax, [1]
cmp rax, 1
jne exit
lea rax, [rax + rcx*1]
cmp rax, 3
jne exit

lea rax, [rcx + rax*1]
cmp rax, 5
jne exit

lea rax, [rax + rcx*1 + 5]
cmp rax, 12
jne exit

lea rax, [rcx + rax*1 + 1]
cmp rax, 15
jne exit

lea rax, [rax]
cmp rax, 15
jne exit

lea rax, [rcx]
cmp rax, 2
jne exit

lea rax, [rcx + 3]
cmp rax, 5
jne exit

lea rax, [rcx + rax * 2]
cmp rax, 12
jne exit

lea rax, [rax + rcx * 4]
cmp rax, 20
jne exit

lea rax, [rax + rcx * 8 + 4]
cmp rax, 40
jne exit

lea rax, [rcx + rax * 4 - 12]
cmp rax, 150
jne exit

lea rax, [rax * 2]
cmp rax, 300
jne exit

lea rax, [rcx * 4]
cmp rax, 8
jne exit

mov rdi, 2
lea eax, [1]
cmp rax, 1
jne exit
lea eax, [rax + rcx*1]
cmp rax, 3
jne exit

lea eax, [rcx + rax*1]
cmp rax, 5
jne exit

lea eax, [rax + rcx*1 + 5]
cmp rax, 12
jne exit

lea eax, [rcx + rax*1 + 1]
cmp rax, 15
jne exit

lea eax, [rax]
cmp rax, 15
jne exit

lea eax, [rcx]
cmp rax, 2
jne exit

lea eax, [rcx + 3]
cmp rax, 5
jne exit

lea eax, [rcx + rax * 2]
cmp rax, 12
jne exit

lea eax, [rax + rcx * 4]
cmp rax, 20
jne exit

lea eax, [rax + rcx * 8 + 4]
cmp rax, 40
jne exit

lea eax, [rcx + rax * 4 - 12]
cmp rax, 150
jne exit

lea eax, [rax * 2]
cmp rax, 300
jne exit

lea eax, [rcx * 4]
cmp rax, 8
jne exit


xor rdi, rdi
exit:
mov rax, 60
syscall
