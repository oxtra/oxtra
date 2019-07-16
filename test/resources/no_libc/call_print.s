.intel_syntax noprefix
.global _start
_start:
push offset hello_world
push [hello_world_size]
call print
pop rax
pop rax
push offset second_string
push [second_string_size]
call print
pop rax
pop rax
call exit



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
	ret


exit:
	mov rax, 0x3c
	syscall

#data-section	
hello_world:
.string "Hello, World!\n"
hello_world_size:
.quad 0x0000000000000e

second_string:
.string "How are you doing? :)\n"
second_string_size:
.quad 0x00000000000016
