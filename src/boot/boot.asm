global start

extern kernel_start
extern kernel_end
extern kernel_main

section .text

bits 32
start:
	mov esp, stack_top

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

