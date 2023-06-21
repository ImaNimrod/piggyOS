bits 32

extern main
global _start

section .text
_start:
    call main
    ret

section .bss
align 4
stack_bottom:
    resb 4096
stack_top:
