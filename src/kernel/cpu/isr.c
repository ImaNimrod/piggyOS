#include <cpu/isr.h>

void init_isr(void) {
	idt_set_descr(0,  (unsigned)isr0,  0x8E);
	idt_set_descr(1,  (unsigned)isr1,  0x8E);
	idt_set_descr(2,  (unsigned)isr2,  0x8E);
	idt_set_descr(3,  (unsigned)isr3,  0x8E);
	idt_set_descr(4,  (unsigned)isr4,  0x8E);
	idt_set_descr(5,  (unsigned)isr5,  0x8E);
	idt_set_descr(6,  (unsigned)isr6,  0x8E);
	idt_set_descr(7,  (unsigned)isr7,  0x8E);
	idt_set_descr(8,  (unsigned)isr8,  0x8E);
	idt_set_descr(9,  (unsigned)isr9,  0x8E);
	idt_set_descr(10, (unsigned)isr10, 0x8E);
	idt_set_descr(11, (unsigned)isr11, 0x8E);
	idt_set_descr(12, (unsigned)isr12, 0x8E);
	idt_set_descr(13, (unsigned)isr13, 0x8E);
	idt_set_descr(14, (unsigned)isr14, 0x8E);
	idt_set_descr(15, (unsigned)isr15, 0x8E);
	idt_set_descr(16, (unsigned)isr16, 0x8E);
	idt_set_descr(17, (unsigned)isr17, 0x8E);
	idt_set_descr(18, (unsigned)isr18, 0x8E);
	idt_set_descr(19, (unsigned)isr19, 0x8E);
	idt_set_descr(20, (unsigned)isr20, 0x8E);
	idt_set_descr(21, (unsigned)isr21, 0x8E);
	idt_set_descr(22, (unsigned)isr22, 0x8E);
	idt_set_descr(23, (unsigned)isr23, 0x8E);
	idt_set_descr(24, (unsigned)isr24, 0x8E);
	idt_set_descr(25, (unsigned)isr25, 0x8E);
	idt_set_descr(26, (unsigned)isr26, 0x8E);
	idt_set_descr(27, (unsigned)isr27, 0x8E);
	idt_set_descr(28, (unsigned)isr28, 0x8E);
	idt_set_descr(29, (unsigned)isr29, 0x8E);
	idt_set_descr(30, (unsigned)isr30, 0x8E);
	idt_set_descr(31, (unsigned)isr31, 0x8E);
}

char *exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};

extern void fault_handler(regs_t *r) {
    if (r->int_no < 32) {
        vga_set_color(VGA_COLOR_RED, VGA_COLOR_LIGHT_GRAY);
        vga_puts("recieved interrupt: ");
		vga_puts(exception_messages[r->int_no]);
    }
}
