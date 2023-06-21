#include <cpu/idt.h>

static idt_descr_t idt[IDT_LEN];
idt_ptr_t idt_ptr;

void idt_set_descr(uint8_t num, uint32_t base, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].seg_sel = 0x08;
    idt[num].zero = 0;
    idt[num].flags = flags; 
    idt[num].base_high = (base >> 16) & 0xFFFF;
}

static void idt_set(idt_ptr_t* idt_ptr) {
    __asm__ volatile("lidtl (idt_ptr)"); 
    __asm__ volatile("sti"); 

    (void) idt_ptr;
}

void init_idt(void) {
    idt_ptr.limit = (sizeof(idt_descr_t) * IDT_LEN) - 1;
    idt_ptr.base = (uintptr_t) &idt;

  	memset(&idt, 0, sizeof(idt_descr_t) * IDT_LEN);

    idt_set(&idt_ptr);
}
