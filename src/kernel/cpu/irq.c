#include <cpu/irq.h>

void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

static void pic_remap(int32_t offset1, int32_t offset2) {
	uint8_t mask1, mask2;
 
	mask1 = inb(PIC1_DATA);                     // save masks
	mask2 = inb(PIC2_DATA);
 
	outb(PIC1_CMD, ICW1_INIT | ICW1_ICW4);  // starts the initialization sequence (in cascade mode)
	io_wait();
	outb(PIC2_CMD, ICW1_INIT | ICW1_ICW4);
	io_wait();
	outb(PIC1_DATA, offset1);                   // ICW2: Master PIC vector offset
	io_wait();
	outb(PIC2_DATA, offset2);                   // ICW2: Slave PIC vector offset
	io_wait();
	outb(PIC1_DATA, 4);                         // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	io_wait();
	outb(PIC2_DATA, 2);                         // ICW3: tell Slave PIC its cascade identity (0000 0010)
	io_wait();
 
	outb(PIC1_DATA, ICW4_8086);
	io_wait();
	outb(PIC2_DATA, ICW4_8086);
	io_wait();
 
	outb(PIC1_DATA, mask1);                        // restore saved masks.
	outb(PIC2_DATA, mask2);
}

static volatile int currentInterrupts[15];

void init_irq(void) {
    pic_remap(0x20, 0x28);

    idt_set_descr(32, irq0,   0x8E);
    idt_set_descr(33, irq1,   0x8E);
    idt_set_descr(34, irq2,   0x8E);
    idt_set_descr(35, irq3,   0x8E);
    idt_set_descr(36, irq4,   0x8E);
    idt_set_descr(37, irq5,   0x8E);
    idt_set_descr(38, irq6,   0x8E);
    idt_set_descr(39, irq7,   0x8E);
    idt_set_descr(40, irq8,   0x8E);
    idt_set_descr(41, irq9,   0x8E);
    idt_set_descr(42, irq10,  0x8E);
    idt_set_descr(43, irq11,  0x8E);
    idt_set_descr(44, irq12,  0x8E);
    idt_set_descr(45, irq13,  0x8E);
    idt_set_descr(46, irq14,  0x8E);
    idt_set_descr(47, irq15,  0x8E);

    for(int i = 0; i <= 15; i++){
        currentInterrupts[i] = 0;
    }

    __asm__ volatile ("sti");
}

void irq_install(int32_t irq, void (*handler)(struct regs *r)) {
    irq_routines[irq] = (void*)handler;
}

void irq_uninstall(int32_t irq) {
    irq_routines[irq] = NULL;
}

static void irq_ack(int irq_no) {
    if (irq_no >= 12) {
        outb(PIC2_CMD, PIC_EOI);
    }
    outb(PIC1_CMD, PIC_EOI);
}

extern void irq_handler(struct regs *r) {
    __asm__ volatile ("cli");

	void (*handler)(struct regs *r);
	if (r->int_no > 47 || r->int_no < 32) {
		handler = NULL;
	} else {
		handler = irq_routines[r->int_no - 32];
	}
	if (handler) {
	    handler(r);
	} else {
	    irq_ack(r->int_no - 32);
	}

    __asm__ volatile ("sti");
}
