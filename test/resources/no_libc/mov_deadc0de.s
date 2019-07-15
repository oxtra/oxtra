.intel_syntax noprefix
.global _start
_start:
MOV RDI, 0xdeadc0de
MOV RAX, 60
SYSCALL