#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stdint.h>

#define IDT_LEN 256

typedef struct idt_descr {
    uint16_t isr_low;
    uint16_t seg_sel;
    uint8_t zero;
    uint8_t flags;
    uint16_t isr_high;
} __attribute__((packed)) idt_descr_t;

typedef struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

extern void* isr_stub_table[];

/* function declarations */

void init_idt(void);
void exception_handler(void);

#endif
