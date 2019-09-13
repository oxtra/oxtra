.intel_syntax noprefix
.global _start
_start:
# store the beginning of the stack
	mov rbp, rsp


# write 0x400 quads onto the stack
	xor rcx, rcx
	mov rbx, 0x0001000100010001
	xor rdx, rdx
	sub rsp, 0x2000
quad_jump:
	mov [rsp + rcx*8], rdx
	add rdx, rbx
	inc rcx
	cmp rcx, 0x400
	jne quad_jump
	
	
# write 0x400 doubles onto the stack
	xor rcx, rcx
	mov ebx, 0x00010001
	xor edx, edx
	sub rsp, 0x1000
double_jump:
	mov [rsp + rcx*4], edx
	add edx, ebx
	inc rcx
	cmp rcx, 0x400
	jne double_jump
	
	
# write 0x400 words onto the stack
	xor rcx, rcx
	mov bx, 0x0001
	xor dx, dx
	sub rsp, 0x800
words_jump:
	mov [rsp + rcx*2], dx
	add dx, bx
	inc rcx
	cmp rcx, 0x400
	jne words_jump
	
	
# write 0x400 bytes onto the stack
	xor rcx, rcx
	mov bl, 0x01
	xor dl, dl
	sub rsp, 0x400
byte_jump:
	mov [rsp + rcx], dl
	add dl, bl
	inc rcx
	cmp rcx, 0x400
	jne byte_jump


# verify the stack-content of the quads
	mov rdi, 0x08
	mov rbx, 0x0001000100010001
	xor rdx, rdx
	xor rcx, rcx
	sub rbp, 0x2000
quad_verify:
	mov rsi, [rbp + rcx*8]
	cmp rsi, rdx
	jne exit
	add rdx, rbx
	inc rcx
	cmp rcx, 0x400
	jne quad_verify
	

# verify the stack-content of the doubles
	mov rdi, 0x04
	mov ebx, 0x00010001
	xor edx, edx
	xor rcx, rcx
	sub rbp, 0x1000
double_verify:
	mov esi, [rbp + rcx*4]
	cmp esi, edx
	jne exit
	add edx, ebx
	inc rcx
	cmp rcx, 0x400
	jne double_verify


# verify the stack-content of the words
	mov rdi, 0x02
	mov bx, 0x0001
	xor dx, dx
	xor rcx, rcx
	sub rbp, 0x800
words_verify:
	mov si, [rbp + rcx*2]
	cmp si, dx
	jne exit
	add dx, bx
	inc rcx
	cmp rcx, 0x400
	jne words_verify


# verify the stack-content of the bytes
	mov rdi, 0x01
	mov bl, 0x01
	xor dl, dl
	xor rcx, rcx
	sub rbp, 0x400
byte_verify:
	mov sil, [rbp + rcx]
	cmp sil, dl
	jne exit
	add dl, bl
	inc rcx
	cmp rcx, 0x400
	jne byte_verify
	

xor rdi, rdi

# exit the program
exit:
	mov rax, 0x3c
	syscall
