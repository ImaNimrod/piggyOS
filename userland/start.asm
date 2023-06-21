bits 32

global _start
extern main

section .text
_start:
    pop eax
    pop ebx
    pop ecx
    push ecx
    push ebx
    push eax
    call main
    ret
