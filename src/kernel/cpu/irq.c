#include <cpu/irq.h>

static void *irq_routines[16] = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

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

void init_irq(void) {
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

}

void irq_install_handler(uint8_t irq, void (*handler)(regs_t *r)) {
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(uint8_t irq) {
    irq_routines[irq] = 0;
}

extern void irq_handler(regs_t *r) {
    void (*handler)(regs_t *r);

    handler = irq_routines[r->int_no - 32];
    if (handler) {
        handler(r);
    }

    if (r->int_no >= 40) {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);

}
