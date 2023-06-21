bits 32

global context_switch

extern current_task

context_switch:
    pop eax
    pushfd
    push eax

    push ebp
    push edi
    push esi
    push ebx

    cli

    mov eax, [current_task]

    mov [eax], esp

    mov eax, [esp + 24] 
    mov ebx, [eax + 8] ; ptr to page_direcory 

    ; virtual address space has no changed
    mov edx, cr3
    cmp edx, ebx
    je .no_pagedir

    mov cr3, ebx

.no_pagedir:
    mov [current_task], eax

    mov esp, [eax]

    pop ebx
    pop esi
    pop edi
    pop ebp
 
    ; setup iretd stack frame properly
    pop eax 
    push cs
    push eax

    iretd
