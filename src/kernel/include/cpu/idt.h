#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stdint.h>
#include <string.h>

#define IDT_LEN 256

typedef struct idt_descr {
    uint16_t base_low;
    uint16_t seg_sel; 
    uint8_t zero;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_descr_t;

typedef struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

typedef struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;    

} regs_t;

/* function declarations */
void init_idt(void);
void idt_set_descr(uint8_t num, uint32_t base, uint8_t flags);

#endif
