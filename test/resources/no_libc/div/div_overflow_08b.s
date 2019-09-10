.intel_syntax noprefix
.global _start

_start:
	mov rax, 0
	mov rdx, 0
	mov rcx, 0

	mov ax, 0x8000
	mov cl, -1
	idiv cl

	mov rbx, 0
	mov bl, al
	mov rdi, rbx

	mov rax, 60
	syscall
