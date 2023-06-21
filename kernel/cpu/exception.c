#include <cpu/exception.h>

static isr_routines isrs_routines[32] = { NULL };

void isrs_install_handler(uint8_t isrs, isr_routines handler) {
	isrs_routines[isrs] = handler;
}

void isrs_uninstall_handler(uint8_t isrs) {
	isrs_routines[isrs] = 0;
}

void exception_init(void) {
	idt_set_descr(0,  (uint32_t) isr0,       0x8E);
	idt_set_descr(1,  (uint32_t) isr1,       0x8E);
	idt_set_descr(2,  (uint32_t) isr2,       0x8E);
	idt_set_descr(3,  (uint32_t) isr3,       0x8E);
	idt_set_descr(4,  (uint32_t) isr4,       0x8E);
	idt_set_descr(5,  (uint32_t) isr5,       0x8E);
	idt_set_descr(6,  (uint32_t) isr6,       0x8E);
	idt_set_descr(7,  (uint32_t) isr7,       0x8E);
	idt_set_descr(8,  (uint32_t) isr8,       0x8E);
	idt_set_descr(9,  (uint32_t) isr9,       0x8E);
	idt_set_descr(10, (uint32_t) isr10,      0x8E);
	idt_set_descr(11, (uint32_t) isr11,      0x8E);
	idt_set_descr(12, (uint32_t) isr12,      0x8E);
	idt_set_descr(13, (uint32_t) isr13,      0x8E);
	idt_set_descr(14, (uint32_t) isr14,      0x8E);
	idt_set_descr(15, (uint32_t) isr15,      0x8E);
	idt_set_descr(16, (uint32_t) isr16,      0x8E);
	idt_set_descr(17, (uint32_t) isr17,      0x8E);
	idt_set_descr(18, (uint32_t) isr18,      0x8E);
	idt_set_descr(19, (uint32_t) isr19,      0x8E);
	idt_set_descr(20, (uint32_t) isr20,      0x8E);
	idt_set_descr(21, (uint32_t) isr21,      0x8E);
	idt_set_descr(22, (uint32_t) isr22,      0x8E);
	idt_set_descr(23, (uint32_t) isr23,      0x8E);
	idt_set_descr(24, (uint32_t) isr24,      0x8E);
	idt_set_descr(25, (uint32_t) isr25,      0x8E);
	idt_set_descr(26, (uint32_t) isr26,      0x8E);
	idt_set_descr(27, (uint32_t) isr27,      0x8E);
	idt_set_descr(28, (uint32_t) isr28,      0x8E);
	idt_set_descr(29, (uint32_t) isr29,      0x8E);
	idt_set_descr(30, (uint32_t) isr30,      0x8E);
	idt_set_descr(31, (uint32_t) isr31,      0x8E);
	idt_set_descr(127,(uint32_t) isr127,     0x8E);
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

void fault_handler(regs_t *r) {
    __asm__ volatile("cli");

    if (r->int_no < 32) {
        vga_set_color(VGA_COLOR_RED, VGA_COLOR_LIGHT_GRAY);
        vga_puts("recieved interrupt: ");
		vga_puts(exception_messages[r->int_no]);
        vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    }
    
    __asm__ volatile("sti");
}
