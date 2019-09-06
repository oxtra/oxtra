.intel_syntax noprefix
.global _start
_start:
mov r9, 10
loop:
dec r9
jz exit
lea r8, [r9+0x30]
mov string, r8b
push offset string
push qword ptr 2
call print
jmp loop

#usage: push ptr, push size
print:
    push rbp
    mov rbp, rsp
    mov rsi, [rbp + 0x18]
    mov rdx, [rbp + 0x10]
    mov rax, 0x01
    mov rdi, 0x01
    syscall
    pop rbp
    ret 0x10

exit:
    mov rax, 0x3c
    syscall
.section .data
string:
.string "0\n"
