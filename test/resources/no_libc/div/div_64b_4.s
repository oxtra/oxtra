.intel_syntax noprefix
.global _start

_start:
	mov rax, 0
	mov rdx, 0
	mov rcx, 0

	mov rax, -10
	mov rdx, 0xffffffff
	mov rcx, -2
	idiv rcx

	mov rbx, 0
	mov rbx, rax
	mov rdi, rbx

	mov rax, 60
	syscall
