.intel_syntax noprefix
.global _start
_start:
MOV RAX, 3
MOV RSI, offset .end - .begin

MOV RBX, 2
MOV RCX, 1

.begin:
ADD RCX, RBX

MOV RDX, RCX
MOV RCX, RBX
MOV RBX, RDX

ADD RAX, 1
MOV RDI, RAX
SHR RDI, 4

IMUL RDI, RSI

ADD RDI, offset .begin

JMP RDI

.end:
MOV RDI, RBX
MOV RAX, 60
SYSCALL
