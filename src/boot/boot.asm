global start

extern kernel_main

section .text

bits 32
start:
    mov esp, stack_top
    push eax
    push ebx

	call kernel_main

	cli
.end:	
    hlt
	jmp .end

section .bss
    align 4096
stack_bottom:
    resb 16384 
stack_top:

