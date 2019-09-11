.intel_syntax noprefix
.global _start

_start:
	mov rax, 0
	mov rdx, 0
	mov rcx, 0

	mov ax, 0
	mov dx, 0x8000
	mov cx, -1
	idiv cx

	mov rbx, 0
	mov bx, ax
	mov rdi, rbx

	mov rax, 60
	syscall
