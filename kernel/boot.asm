global start

extern kernel_main

section .multiboot_header

header_start:
	dd 0xe85250d6 
	dd 0 
	dd header_end - header_start
	dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

	dw 0
	dw 0
	dd 8
header_end:

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
