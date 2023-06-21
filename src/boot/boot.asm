global start
bits 32
section .text
extern kernel_main
start:
	mov esp, stack_top

    call kernel_main
.end:
    jmp .end

section .bss 
stack_bottom:
    resb 0x4000
stack_top:
