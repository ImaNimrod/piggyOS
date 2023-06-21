bits 32

global _start
extern main

section .text
_start:
    call main
    ret
