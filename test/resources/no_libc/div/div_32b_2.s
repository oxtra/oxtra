.intel_syntax noprefix
.global _start

_start:
	mov rax, 0
	mov rdx, 0
	mov rcx, 0

	mov eax, -10
	mov edx, 0xffff
	mov ecx, 2
	idiv ecx

	mov rbx, 0
	mov ebx, eax
	mov rdi, rbx

	mov rax, 60
	syscall
