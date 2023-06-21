#include <cpu/irq.h>

typedef void (*irq_routines) (regs_t *r);
static irq_routines irqs[16] = {0};

static void irq_remap(void) {
    outb(PIC1_CMD,  0x11);
    outb(PIC2_CMD,  0x11);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0x0);
    outb(PIC2_DATA, 0x0);
}

static volatile int current_int[15];

void irq_init(void) {
    irq_remap();

    idt_set_descr(32, (unsigned)irq0,  0x8E);
    idt_set_descr(33, (unsigned)irq1,  0x8E);
    idt_set_descr(34, (unsigned)irq2,  0x8E);
    idt_set_descr(35, (unsigned)irq3,  0x8E);
    idt_set_descr(36, (unsigned)irq4,  0x8E);
    idt_set_descr(37, (unsigned)irq5,  0x8E);
    idt_set_descr(38, (unsigned)irq6,  0x8E);
    idt_set_descr(39, (unsigned)irq7,  0x8E);
    idt_set_descr(40, (unsigned)irq8,  0x8E);
    idt_set_descr(41, (unsigned)irq9,  0x8E);
    idt_set_descr(42, (unsigned)irq10, 0x8E);
    idt_set_descr(43, (unsigned)irq11, 0x8E);
    idt_set_descr(44, (unsigned)irq12, 0x8E);
    idt_set_descr(45, (unsigned)irq13, 0x8E);
    idt_set_descr(46, (unsigned)irq14, 0x8E);
    idt_set_descr(47, (unsigned)irq15, 0x8E);

    for(int i = 0; i < 15; i++){
        current_int[i] = 0;
    }

    __asm__ volatile("sti");
}

void irq_wait(uint8_t irq) {
    while (!current_int[irq]){};
    current_int[irq] = 0;
}

void irq_install_handler(uint8_t irq, irq_routines handler) {
    irqs[irq] = handler;
}

void irq_uninstall_handler(uint8_t irq) {
    irqs[irq] = 0;
}

void irq_ack(uint8_t irq) {
    if (irq >= 8)
        outb(PIC1_CMD, PIC_EOI);
		outb(PIC2_CMD, PIC_EOI);
	outb(PIC1_CMD, PIC_EOI);
}

void irq_handler(regs_t *r) {
    __asm__ volatile("cli");

    current_int[r -> int_no - 32] = 1;
    irq_routines handler;

    if (r->int_no > 47 || r->int_no < 32) {
		handler = NULL;
	} else {
		handler = irqs[r->int_no - 32];
	}
	if (handler) {
		handler(r);
	} else {
		irq_ack(r->int_no - 32);
	}

    __asm__ volatile("sti");
}

void enable_int(void) {
    __asm__ volatile("sti");
}

void disable_int(void) {
    __asm__ volatile("cli");
}

void irq_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}
 
void irq_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}
