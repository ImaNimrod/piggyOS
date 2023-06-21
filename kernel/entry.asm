global start 

extern low_kernel_start
extern low_kernel_end

extern kernel_start
extern kernel_end

extern kernel_main

KERNEL_BASE equ 0xC0000000
PAGE_SIZE   equ 4096
PAGE_SHIFT  equ 12 

bits 32

section .multiboot2_header

header_start:
	dd 0xe85250d6 
	dd 0 
	dd header_end - header_start
	dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))

	dw 0
	dw 0
	dd 8
header_end:

section .low_text progbits

start:
    ; save multiboot info for kernel
    mov [multiboot_magic], eax
    mov [multiboot_info],  ebx

    ; identity map from 0x00000000 to end of low kernel mem
    mov eax, lowmem_pt
    mov [page_directory], eax
    or dword [page_directory], 0x003 

    xor eax, eax 
.low_mem:
    ; copy physical addr to PTE and mark it as present, writable
    mov ecx, eax
    shr ecx, PAGE_SHIFT     
    and ecx, 0x3ff          
    mov [lowmem_pt+ecx*4], eax 
    or dword [lowmem_pt+ecx*4], 0x003 

    ; check if we have finished identity mapping low mem
    add eax, PAGE_SIZE      
    cmp eax, low_kernel_end 
    jl .low_mem

    ; create kernel mappings in higher half
    mov edx, KERNEL_BASE
    shr edx, 22          

    ; mark PT as present, writable
    mov eax, kernel_pt
    mov [page_directory+edx*4], eax
    or dword [page_directory+edx*4], 0x03 

    mov eax, kernel_start 
.higher:
    ; generate kernel pte
    mov ecx, eax
    shr ecx, PAGE_SHIFT
    and ecx, 0x3ff 

    ; convert virt addr to phys addr
    mov ebx, eax 
    sub ebx, KERNEL_BASE 
    mov [kernel_pt+ecx*4], ebx
    or dword [kernel_pt+ecx*4], 0x003 

    ; check if we are done mapping the kernel
    add eax, PAGE_SIZE 
    cmp eax, kernel_end 
    jl .higher

    ; load cr3 with our page directory
    mov eax, page_directory
    mov cr3, eax 

    ; enable 4MB pages
    mov ecx, cr4;
    or ecx, 0x002
    mov cr4, ecx

    ; enable pagaing
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax 

    ; initialize stack ptr and pass multiboot info to kernel 
    mov esp, stack_top 

    push dword [multiboot_magic]        
    push dword [multiboot_info] 

    ; run kernel
    call kernel_main 

    ; infinite loop for if kernel returns (it won't)
    cli
.end:	
    hlt
	jmp .end

section .low_bss nobits

align 4096
page_directory:
  resd 1024       
lowmem_pt:
  resd 1024 
kernel_pt:
  resd 1024

section .low_data

multiboot_magic:
  dd 0
multiboot_info:
  dd 0

section .bss
    align 4
stack_bottom:
    resb 16384 
stack_top:
