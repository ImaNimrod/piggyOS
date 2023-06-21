%macro isr_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret 
%endmacro

; if writing for 64-bit, use iretq instead
%macro isr_no_err_stub 1
isr_stub_%+%1:
    call exception_handler
    iret
%endmacro

extern exception_handler
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31

global isr_stub_table
isr_stub_table:
%assign i 0 
%rep    32 
    dd isr_stub_%+i ; use DQ instead if targeting 64-bit
%assign i i+1 
%endrep

global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15


irq0:
	cli
	push byte 0
	push byte 32
	jmp irq_common_stub
irq1:
	cli
	push byte 0
	push byte 33
	jmp irq_common_stub
irq2:
	cli
	push byte 0
	push byte 34
	jmp irq_common_stub
irq3:
	cli
	push byte 0
	push byte 35
	jmp irq_common_stub
irq4:
	cli	
	push byte 0
	push byte 36
	jmp irq_common_stub
irq5:
	cli
	push byte 0
	push byte 37
	jmp irq_common_stub
irq6:
	cli
	push byte 0
	push byte 38
	jmp irq_common_stub
irq7:
	cli
	push byte 0
	push byte 39
	jmp irq_common_stub
irq8:
	cli
	push byte 0
	push byte 40
	jmp irq_common_stub
irq9:
	cli
	push byte 0
	push byte 41
	jmp irq_common_stub
irq10:
	cli
	push byte 0
	push byte 42
	jmp irq_common_stub
irq11:
	cli
	push byte 0
	push byte 43
	jmp irq_common_stub
irq12:
	cli
	push byte 0
	push byte 44
	jmp irq_common_stub
irq13:
	cli
	push byte 0
	push byte 45
	jmp irq_common_stub
irq14:
	cli
	push byte 0
	push byte 46
	jmp irq_common_stub
irq15:
	cli
	push byte 0
	push byte 47
	jmp irq_common_stub

[extern] irq_handler

irq_common_stub:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov eax, esp
	push eax
	mov eax, irq_handler
	call eax
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8
	iret
