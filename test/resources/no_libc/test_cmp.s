.intel_syntax noprefix
.global _start
_start:
mov rdi, 1
mov rax, 13
cmp rax, 12 # 13 > 12
jle exit
jbe exit
cmp rax, 13 # 13 == 13
jne exit
cmp rax, 14 # 13 < 14
jge exit
jae exit

mov rcx, 0
mov rax, 1
cmp rax, rcx # 1 > 0
jna exit
jng exit
mov rax, 0
cmp rax, rcx # 0 == 0
jne exit
mov rax, -1
cmp rax, rcx # -1 < 0 - max > 0
jna exit
jnl exit

mov rcx, 0x8000000000000000
mov rax, 0x7FFFFFFFFFFFFFFF
cmp rax, rcx # max > -max - rax < rcx
jae exit
jle exit
inc rax
cmp rax, rcx # rax == rcx
jne exit
inc rax # rax > rcx
cmp rax, rcx
jbe exit
jle exit

mov rdi, 2
mov eax, 13
cmp eax, 12 # 13 > 12
jle exit
jbe exit
cmp eax, 13 # 13 == 13
jne exit
cmp eax, 14 # 13 < 14
jge exit
jae exit

mov ecx, 0
mov eax, 1
cmp eax, ecx # 1 > 0
jna exit
jng exit
mov eax, 0
cmp eax, ecx # 0 == 0
jne exit
mov eax, -1
cmp eax, ecx # -1 < 0 - max > 0
jna exit
jnl exit

mov ecx, 0x80000000
mov eax, 0x7FFFFFFF
cmp eax, ecx # max > -max - rax < rcx
jae exit
jle exit
inc eax
cmp eax, ecx # rax == rcx
jne exit
inc eax 
cmp eax, ecx # rax > rcx
jbe exit
jle exit

mov rdi, 3
mov ax, 13
cmp ax, 12 # 13 > 12
jle exit
jbe exit
cmp ax, 13 # 13 == 13
jne exit
cmp ax, 14 # 13 < 14
jge exit
jae exit

mov cx, 0
mov ax, 1
cmp ax, cx # 1 > 0
jna exit
jng exit
mov ax, 0
cmp ax, cx # 0 == 0
jne exit
mov ax, -1
cmp ax, cx # -1 < 0 - max > 0
jna exit
jnl exit

mov cx, 0x8000
mov ax, 0x7FFF
cmp ax, cx # max > -max - rax < rcx
jae exit
jle exit
inc ax
cmp ax, cx # rax == rcx
jne exit
inc ax 
cmp ax, cx # rax > rcx
jbe exit
jle exit

mov rdi, 4
mov al, 13
cmp al, 12 # 13 > 12
jle exit
jbe exit
cmp al, 13 # 13 == 13
jne exit
cmp al, 14 # 13 < 14
jge exit
jae exit

mov cl, 0
mov al, 1
cmp al, cl # 1 > 0
jna exit
jng exit
mov al, 0
cmp al, cl # 0 == 0
jne exit
mov al, -1
cmp al, cl # -1 < 0 - max > 0
jna exit
jnl exit

mov cl, 0x80
mov al, 0x7F
cmp al, cl # max > -max - rax < rcx
jae exit
jle exit
inc al
cmp al, cl # rax == rcx
jne exit
inc al 
cmp al, cl # rax > rcx
jbe exit
jle exit

mov rdi, 5
mov ah, 13
cmp ah, 12 # 13 > 12
jle exit
jbe exit
cmp ah, 13 # 13 == 13
jne exit
cmp ah, 14 # 13 < 14
jge exit
jae exit

mov ch, 0
mov ah, 1
cmp ah, ch # 1 > 0
jna exit
jng exit
mov ah, 0
cmp ah, ch # 0 == 0
jne exit
mov ah, -1
cmp ah, ch # -1 < 0 - max > 0
jna exit
jnl exit

mov ch, 0x80
mov ah, 0x7F
cmp ah, ch # max > -max - rax < rcx
jae exit
jle exit
inc ah
cmp ah, ch # rax == rcx
jne exit
inc ah 
cmp ah, ch # rax > rcx
jbe exit
jle exit

xor rdi, rdi

exit:
mov rax, 0x3C
syscall
