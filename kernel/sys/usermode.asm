bits 32

global switch_to_user

switch_to_user:
    cli

    pushfd
    pop ebx
    or ebx, 0x200  ; IF = 1

    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov   eax, [esp + 4]
    push  0x23         ; SS
    push  0xBF8FFFFB   ; ESP
    push  ebx          ; EFLAGS
    push  0x1B        ; CS
    push  eax         ; EIP
    ;xchg  bx, bx      ; Kernel Stack with User values ---^
    iret              ; force load User segments/registers
