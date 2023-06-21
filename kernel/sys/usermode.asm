bits 32

global enter_usermode

enter_usermode:
    cli
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, esp
    push dword 0x23
    push eax
    pushf
    push dword 0x1b
    push dword [next]
    iret
next:
