.intel_syntax noprefix
.global _start

_start:
	mov rax, 0
	mov rdx, 0
	mov rcx, 0

	mov ax, -10
	mov dx, 0xff
	mov cx, 2
	idiv cx

	mov rbx, 0
	mov bx, ax
	mov rdi, rbx

	mov rax, 60
	syscall
