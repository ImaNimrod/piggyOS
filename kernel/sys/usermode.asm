bits 32

global user_regs_switch
global kernel_regs_switch

user_regs_switch:
    mov ebp, [esp + 4]
    mov ecx, [ebp + 4]
    mov edx, [ebp + 8]
    mov ebx, [ebp + 12]
    mov esi, [ebp + 24]
    mov edi, [ebp + 28]
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push 0x23
    mov eax, [ebp + 16]
    push eax
    mov eax, [ebp + 32]
    push eax
    push 0x1b
    mov eax, [ebp + 40]
    push eax

    mov eax, [ebp + 0]
    mov ebp, [ebp + 20]
    iret

kernel_regs_switch:
    mov ebp, [esp + 4]
    mov ecx, [ebp + 4]
    mov edx, [ebp + 8]
    mov ebx, [ebp + 12]
    mov esi, [ebp + 24]
    mov edi, [ebp + 28]
    mov eax, [ebp + 32]
    push eax
    popfd

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    push 0x10
    mov eax, [ebp + 16]
    push eax
    pushfd
    push 0x08
    mov eax, [ebp + 40]
    push eax

    mov eax, [ebp + 0]
    mov ebp, [ebp + 20]
    sti
    iret

