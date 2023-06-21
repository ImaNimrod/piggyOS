bits 32 

section .multiboot2_header
align 8
header_start:
	dd 0xe85250d6 
	dd 0 
	dd header_end - header_start
	dd -(0xe85250d6 + 0 + (header_end - header_start))

    dw 0 
    dw 0 
    dd 8 
header_end:

section .bss
align 4
stack_bottom:
    resb 0x1000
stack_top:

KERNEL_VIRTUAL_BASE equ 0xc0000000
KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)  ; PDE of kernel's 4MB PTE 

section .data
align 0x1000
global boot_page_dir
boot_page_dir:
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0
    dd 0x00000083
    times (1024 - KERNEL_PAGE_NUMBER - 1) dd 0

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
    invlpg [0]

    ; initialize stack
    mov esp, stack_top

    ; pass multiboot information to kernel
    add ebx, KERNEL_VIRTUAL_BASE
    push ebx
    push eax

    ; load our kernel
    call kernel_main

    cli
.end:
    hlt
    jmp .end
