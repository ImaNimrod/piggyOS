#ifndef _KERNEL_IDT_H
#define _KERNEL_IDT_H

#include <stdint.h>
#include <string.h>

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

struct regs {
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
};

extern void* isr_stub_table[];

/* function declarations */

void init_idt(void);
void idt_set_descr(uint8_t num, void* isr, uint8_t flags);
void exception_handler(void);

#endif
