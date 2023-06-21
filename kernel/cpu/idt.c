#include <cpu/desc_tbls.h>

static idt_descr_t idt[IDT_LEN];
static idt_ptr_t idt_ptr;

void idt_set_descr(uint8_t num, uint32_t base, uint8_t flags) {
    idt[num].base_low = base & 0xffff;
    idt[num].base_high = (base >> 16) & 0xffff;
    idt[num].seg_sel = 0x08;
    idt[num].zero = 0;
    idt[num].flags = flags; 
}

static void idt_set(void) {
    __asm__ volatile("lidt %0" : : "m"(idt_ptr));
}

void idt_init(void) {
    idt_ptr.limit = sizeof(idt_descr_t) * IDT_LEN - 1;
    idt_ptr.base = (uintptr_t) &idt;

  	memset(&idt, 0, sizeof(idt_descr_t) * IDT_LEN);

    /* PIC initialization */
    outb(PIC1_CMD,  0x11);
    outb(PIC2_CMD,  0x11);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);
    outb(PIC1_DATA, 0x00);
    outb(PIC2_DATA, 0x00);

    /* exception stubs */
	idt_set_descr(0,  (uint32_t) &isr0,   0x8E);
	idt_set_descr(1,  (uint32_t) &isr1,   0x8E);
	idt_set_descr(2,  (uint32_t) &isr2,   0x8E);
	idt_set_descr(3,  (uint32_t) &isr3,   0x8E);
	idt_set_descr(4,  (uint32_t) &isr4,   0x8E);
	idt_set_descr(5,  (uint32_t) &isr5,   0x8E);
	idt_set_descr(6,  (uint32_t) &isr6,   0x8E);
	idt_set_descr(7,  (uint32_t) &isr7,   0x8E);
	idt_set_descr(8,  (uint32_t) &isr8,   0x8E);
	idt_set_descr(9,  (uint32_t) &isr9,   0x8E);
	idt_set_descr(10, (uint32_t) &isr10,  0x8E);
	idt_set_descr(11, (uint32_t) &isr11,  0x8E);
	idt_set_descr(12, (uint32_t) &isr12,  0x8E);
	idt_set_descr(13, (uint32_t) &isr13,  0x8E);
	idt_set_descr(14, (uint32_t) &isr14,  0x8E);
	idt_set_descr(15, (uint32_t) &isr15,  0x8E);
	idt_set_descr(16, (uint32_t) &isr16,  0x8E);
	idt_set_descr(17, (uint32_t) &isr17,  0x8E);
	idt_set_descr(18, (uint32_t) &isr18,  0x8E);
	idt_set_descr(19, (uint32_t) &isr19,  0x8E);
	idt_set_descr(20, (uint32_t) &isr20,  0x8E);
	idt_set_descr(21, (uint32_t) &isr21,  0x8E);
	idt_set_descr(22, (uint32_t) &isr22,  0x8E);
	idt_set_descr(23, (uint32_t) &isr23,  0x8E);
	idt_set_descr(24, (uint32_t) &isr24,  0x8E);
	idt_set_descr(25, (uint32_t) &isr25,  0x8E);
	idt_set_descr(26, (uint32_t) &isr26,  0x8E);
	idt_set_descr(27, (uint32_t) &isr27,  0x8E);
	idt_set_descr(28, (uint32_t) &isr28,  0x8E);
	idt_set_descr(29, (uint32_t) &isr29,  0x8E);
	idt_set_descr(30, (uint32_t) &isr30,  0x8E);
	idt_set_descr(31, (uint32_t) &isr31,  0x8E);
	idt_set_descr(127,(uint32_t) &isr127, 0x8E);

    /* IRQ stubs */
    idt_set_descr(32, (uint32_t) &irq0,  0x8E);
    idt_set_descr(33, (uint32_t) &irq1,  0x8E);
    idt_set_descr(34, (uint32_t) &irq2,  0x8E);
    idt_set_descr(35, (uint32_t) &irq3,  0x8E);
    idt_set_descr(36, (uint32_t) &irq4,  0x8E);
    idt_set_descr(37, (uint32_t) &irq5,  0x8E);
    idt_set_descr(38, (uint32_t) &irq6,  0x8E);
    idt_set_descr(39, (uint32_t) &irq7,  0x8E);
    idt_set_descr(40, (uint32_t) &irq8,  0x8E);
    idt_set_descr(41, (uint32_t) &irq9,  0x8E);
    idt_set_descr(42, (uint32_t) &irq10, 0x8E);
    idt_set_descr(43, (uint32_t) &irq11, 0x8E);
    idt_set_descr(44, (uint32_t) &irq12, 0x8E);
    idt_set_descr(45, (uint32_t) &irq13, 0x8E);
    idt_set_descr(46, (uint32_t) &irq14, 0x8E);
    idt_set_descr(47, (uint32_t) &irq15, 0x8E);

    idt_set();

    /* renable interrupts */
    __asm__ volatile("sti");
}
