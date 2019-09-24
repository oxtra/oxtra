.intel_syntax noprefix
.global _start
_start:

mov rdi, 1
mov r15, 0x123456789abcdef
xor rax, rax
setz r15b
cmp r15b, 1
jne exit
mov rcx, 0x123456789abcdef
xor rax, rax
setz ch
cmp ch, 1
jne exit
inc rax
mov r12, offset data
setz [r12 + rax * 4 + 1]
cmp byte ptr [r12 + rax * 4 + 1], 0
jne exit

mov rdi, 2
mov al, 0x7f
inc al
seto al
cmp al, 1
jne exit
seto al
cmp al, 0
jne exit

mov rdi, 3
mov al, 0x7f
inc al
setno al
cmp al, 0
jne exit
setno al
cmp al, 1
jne exit

mov rdi, 4
cmp rdi, 5
setb al
cmp al, 1
jne exit
cmp rdi, 4
setb al
cmp al, 0
jne exit

mov rdi, 5
cmp rdi, 6
setnb al
cmp al, 0
jne exit
cmp rdi, 5
setnb al
cmp al, 1
jne exit

mov rdi, 6
cmp rdi, 6
sete al
cmp al, 1
jne exit
cmp rdi, 0
sete al
cmp al, 0
jne exit

mov rdi, 7
cmp rdi, 7
setne al
cmp al, 0
jne exit
cmp rdi, 0
setne al
cmp al, 1
jne exit

mov rdi, 8
cmp rdi, 8
setbe al
cmp al, 1
jne exit
cmp rdi, 7
setbe al
cmp al, 0
jne exit

mov rdi, 9
cmp rdi, 9
setnbe al
cmp al, 0
jne exit
cmp rdi, 8
setnbe al
cmp al, 1
jne exit

mov rdi, 10
cmp rdi, 100
sets al
cmp al, 1
jne exit
cmp rdi, 0
sets al
cmp al, 0
jne exit

mov rdi, 11
cmp rdi, 100
setns al
cmp al, 0
jne exit
cmp rdi, 0
setns al
cmp al, 1
jne exit

mov rdi, 12
xor r9, r9
inc r9
setpo al
cmp al, 1
jne exit
add r9, 2
setpo al
cmp al, 0
jne exit

mov rdi, 13
xor r9, r9
inc r9
setpe al
cmp al, 0
jne exit
add r9, 2
setpe al
cmp al, 1
jne exit

mov rdi, 14
mov r9, -1
cmp r9, 0
setl al
cmp al, 1
jne exit
cmp r9, -1
setl al
cmp al, 0
jne exit

mov rdi, 15
mov r9, -1
cmp r9, 0
setnl al
cmp al, 0
jne exit
cmp r9, -1
setnl al
cmp al, 1
jne exit

mov rdi, 16
mov r9, -1
cmp r9, 0
setle al
cmp al, 1
jne exit
cmp r9, -2
setle
cmp al, 0
jne exit

mov rdi, 17
mov r9, -1
cmp r9, 0
setnle al
cmp al, 0
jne exit
cmp r9, -2
setnle
cmp al, 1
jne exit


xor rdi, rdi
exit:
mov rax, 60
syscall

.data
data: .8byte 0x123456789abcdef
