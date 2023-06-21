#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stdint.h>
#include <string.h>

#define IDT_LEN 256

typedef struct {
    uint16_t base_low;
    uint16_t seg_sel; 
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_descr_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

/* function declarations */
void idt_init(void);
void idt_set_descr(uint8_t num, uint32_t base, uint8_t flags);

#endif
