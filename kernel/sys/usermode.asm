bits 32

global switch_to_user

switch_to_user:
    cli
    
    ; set data segement values to point to user data segment with rpl=3
    mov ax, 0x23
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

	mov eax, [esp + 4]

	; set address in user stack which causes the page fault when finishing a user thread
	sub eax, 4
	mov ebx, [esp + 12]
	mov dword [eax], ebx

    mov ebx, [esp + 4] ; eip address to jump to

    ; set up iret stack frame
    push 0x23       ; stack segment selector (user data with rpl=3) 
    push eax        ; stack pointer 
    pushfd          ; eflags

    pop eax
    or eax, 0x200   ; enable interrupts in eflags
    push eax 

    push 0x1b       ; code segment selector (user code with rpl=3) 
    push ebx 
    iret
