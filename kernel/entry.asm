bits 32 

section .multiboot2_header
align 8
header_start:
	dd 0xe85250d6 
	dd 0 
	dd header_end - header_start
	dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

	dw 0
	dw 0
	dd 8
header_end:

section .bss
align 4
kernel_stack:
    resb 0x4000
stack_top:

KERNEL_VIRTUAL_BASE equ 0xc0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; PDE of kernel's 4MB PTE 

section .data
align 0x1000
global boot_page_dir
boot_page_dir:
    dd 0x00000083
    dd 0x00400083 
    dd 0x00800083 
    dd 0x00c00083 
    dd 0x01000083 
    dd 0x01400083 
    times (KERNEL_PAGE_NUMBER - 6) dd 0
    dd 0x00000083
    %assign i 0xc0400083
    %rep 0x7f 
        dd i 
        %assign i i+0x400000
    %endrep


section .text
global start
start:
    ; load cr3 with our page directory
    mov ecx, (boot_page_dir - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx

    ; ; enable 4MB pages
    mov ecx, cr4
    or ecx, 0x00000010
    mov cr4, ecx

    ; enable paging
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx

    ; jump to the absoulute address of our kernel in higher mem
    lea ecx, [higher_half]
    jmp ecx

extern kernel_main
higher_half:
    ; disable interrupts until it is safe
    cli
    cld

    ; unmap the first identity-mapped 4MB; we dont need it anymore
    mov dword [boot_page_dir], 0
    invlpg [0]

    ; initialize stack
    mov esp, stack_top
    push esp

    ; pass multiboot information to kernel
    add ebx, KERNEL_VIRTUAL_BASE
    push ebx
    push eax

    ; load out kernel
    call kernel_main

    cli
.end:
    hlt
    jmp .end
