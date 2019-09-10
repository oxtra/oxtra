.intel_syntax noprefix
.global _start

_start:
	mov rax, 0
	mov rdx, 0
	mov rcx, 0

	mov eax, 0
	mov edx, 0x80000000
	mov ecx, -1
	idiv ecx

	mov rbx, 0
	mov ebx, eax
	mov rdi, rbx

	mov rax, 60
	syscall
