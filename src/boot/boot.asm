bits 32

extern kernel_start
extern kernel_end
extern kernel_main

global start

section .page_setup
start:
    mov dword edi, boot_page_tbl1 - 0xC0000000 

    mov dword esi, 0
    mov dword ecx, 1023

map_kernel:
    cmp dword esi, kernel_start 
	jl setup_page_entry
    cmp dword esi, kernel_end - 0xC0000000 
	jge map_vga

    mov dword edx, esi
	or dword edx, 0x003
    mov dword [edi], edx

setup_page_entry:
    add dword esi, 4096
    add dword edi, 4
	loop map_kernel 

map_vga:
    mov dword [boot_page_tbl1 - 0xC0000000 + 1023 * 4], 0x000B8003

    mov dword [boot_page_directory - 0xC0000000 ], boot_page_tbl1 - 0xC0000000 + 0x003
    mov dword [boot_page_directory - 0xC0000000 + 768 * 4], boot_page_tbl1 - 0xC0000000 + 0x003

    mov dword ecx, boot_page_directory - 0xC0000000
    mov cr3, ecx

    mov ecx, cr0
    or dword ecx, 0x80010000
    mov cr0, ecx

	lea ecx, kernel_entry
	jmp ecx

section .text

kernel_entry:
    mov dword [boot_page_directory], 0

    mov ecx, cr3
	mov cr3, ecx

	mov esp, stack_top

	call kernel_main

	cli
.end:	
    hlt
	jmp .end

section .bss
    align 4096
boot_page_directory:
    resb 4096
boot_page_tbl1:
    resb 4096
stack_bottom:
    resb 16384 
stack_top:

