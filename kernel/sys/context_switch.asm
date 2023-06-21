bits 32

global context_switch

extern current_task

context_switch:
    pop     eax		; convert CALL stack frame (EIP only)...
    pushf		      ; ...to partial IRET (EIP, EFLAGS)
    push    eax

    push    ebp	  ; callee-save registers used by C
    push    edi	  ; pushing these creates kregs_t stack frame
    push    esi
    push    ebx
    cli
    mov     eax, [current_task]

    ; store current kernel ESP in thread_t struct of current thread/task
    mov     [eax], esp

    ; get pointer (thread_t t) to new task/thread to run
    mov     eax,  [esp + 24]

    mov     ebx, [eax + 8]
    ; cmp     ebx, dword 0
    ; je      no_cr3

    xchg bx, bx
    mov     cr3, ebx

no_cr3:
    mov     [current_task], eax

    mov     esp,[eax]
    pop     ebx
    pop     esi
    pop     edi
    pop     ebp

    pop     eax
    push    cs
    push    eax

    iret		; IRET to new kernel thread
