.intel_syntax noprefix
.global _start

_start:
	mov rax, 0
	mov rdx, 0
	mov rcx, 0

	mov ax, 10
	mov cl, 0
	div cl

	mov rbx, 0
	mov bl, al
	mov rdi, rbx

	mov rax, 60
	syscall
