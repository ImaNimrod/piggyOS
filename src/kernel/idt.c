#include <idt.h>

__attribute__((aligned(0x10))) 
static idt_descr_t idt[IDT_LEN];
idt_ptr_t idt_ptr;

static void idt_set_descr(idt_descr_t* descr, void* isr, uint8_t flags) {
    descr->isr_low    = (uint32_t) isr & 0xFFFF;
    descr->seg_sel    = 0x01; 
    descr->flags      = flags;
    descr->isr_high   = (uint32_t) isr >> 16;
    descr->zero       = 0;
}


static void idt_set(idt_ptr_t* idt_ptr) {
    __asm__ volatile("lidtl (idt_ptr)"); 
    __asm__ volatile("sti"); 

    (void) idt_ptr;
}

void init_idt(void) {
    for (uint8_t vector = 0; vector < 32; vector++) {
        idt_set_descr(&idt[vector], isr_stub_table[vector], 0x8E);
    }
 
    idt_ptr.limit = (uint16_t) sizeof(idt_descr_t) * IDT_LEN - 1;
    idt_ptr.base = (uintptr_t) &idt;

    idt_set(&idt_ptr);
}

void exception_handler() {
    __asm__ volatile("cli; hlt"); 
}
