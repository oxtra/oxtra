.intel_syntax noprefix
.global _start
_start:
# store the beginning of the stack
	mov rbp, rsp


# write 0x400 quads onto the stack
	xor rcx, rcx
	mov rbx, 0x0001000100010001
	xor rdx, rdx
quad_jump:
	mov [rsp + rcx*8 - 8], rdx
	add rdx, rbx
	dec rcx
	cmp rcx, -0x400
	jne quad_jump
	sub rsp, 0x2000
	
	
# write 0x400 doubles onto the stack
	xor rcx, rcx
	mov ebx, 0x00010001
	xor edx, edx
double_jump:
	mov [rsp + rcx*4 - 4], edx
	add edx, ebx
	dec rcx
	cmp rcx, -0x400
	jne double_jump
	sub rsp, 0x1000
	
	
# write 0x400 words onto the stack
	xor rcx, rcx
	mov bx, 0x0001
	xor dx, dx
words_jump:
	mov [rsp + rcx*2 - 2], dx
	add dx, bx
	dec rcx
	cmp rcx, -0x400
	jne words_jump
	sub rsp, 0x800
	
	
# write 0x400 bytes onto the stack
	xor rcx, rcx
	mov bl, 0x01
	xor dl, dl
byte_jump:
	mov [rsp + rcx - 1], dl
	add dl, bl
	dec rcx
	cmp rcx, -0x400
	jne byte_jump


# verify the stack-content of the quads
	mov rdi, 0x08
	mov rbx, 0x0001000100010001
	xor rdx, rdx
	xor rcx, rcx
quad_verify:
	mov rsi, [rbp + rcx*8 - 8]
	cmp rsi, rdx
	jne exit
	add rdx, rbx
	dec rcx
	cmp rcx, -0x400
	jne quad_verify
	sub rbp, 0x2000
	

# verify the stack-content of the doubles
	mov rdi, 0x04
	mov ebx, 0x00010001
	xor edx, edx
	xor rcx, rcx
double_verify:
	mov esi, [rbp + rcx*4 - 4]
	cmp esi, edx
	jne exit
	add edx, ebx
	dec rcx
	cmp rcx, -0x400
	jne double_verify
	sub rbp, 0x1000


# verify the stack-content of the words
	mov rdi, 0x02
	mov bx, 0x0001
	xor dx, dx
	xor rcx, rcx
words_verify:
	mov si, [rbp + rcx*2 - 2]
	cmp si, dx
	jne exit
	add dx, bx
	dec rcx
	cmp rcx, -0x400
	jne words_verify
	sub rbp, 0x800


# verify the stack-content of the bytes
	mov rdi, 0x01
	mov bl, 0x01
	xor dl, dl
	xor rcx, rcx
byte_verify:
	mov sil, [rbp + rcx - 1]
	cmp sil, dl
	jne exit
	add dl, bl
	dec rcx
	cmp rcx, -0x400
	jne byte_verify
	

# exit the program
exit:
	xor rdi, rdi
	mov rax, 0x3c
	syscall

