bits 32

global context_switch

extern current_task

; void context_switch(task_t* next);
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

    iret
